#include <Commands/Play.hpp>
#include <dpp/dpp.h>
#include <dpp/nlohmann/json.hpp>
#include <string>
#include <filesystem>
#include <ctime>

using json = nlohmann::json;

std::string getResultFromCommand(std::string cmd) {
	std::string result;
	FILE* stream;
	const int maxBuffer = 256; // 버퍼의 크기는 적당하게
	char buffer[maxBuffer];
	cmd.append(" 2>&1"); // 표준에러를 표준출력으로 redirect

    stream = popen(cmd.c_str(), "r"); // 주어진 command를 shell로 실행하고 파이프 연결 (fd 반환)
    	if (stream) {
    		while (fgets(buffer, maxBuffer, stream) != NULL) result.append(buffer); // fgets: fd (stream)를 길이 (maxBuffer)만큼 읽어 버퍼 (buffer)에 저장
    		pclose(stream); // 파이프 닫는 것 잊지 마시고요!
    	}
	return result;
}

commands::Play::Play(dpp::snowflake botID, BumbleCeepp* Bot)
 : ICommand(botID, Bot)
{
    dpp::slashcommand command = dpp::slashcommand("p", "노래 재생", botID);

    command.add_option(
        dpp::command_option(dpp::co_string, "query", "링크 또는 검색어", botID)
    );

    commandObjectVector.push_back(command);
}

void commands::Play::operator()(const dpp::slashcommand_t& event)
{
    if (std::holds_alternative<std::monostate>(event.get_parameter("query")))
    {
        event.reply("노래를 재생하려면 검색어 또는 링크를 입력해 주십시오.");
        return;
    }

    /* Attempt to connect to a voice channel, returns false if we fail to connect. */
    if (!event.from->get_voice(event.command.guild_id))
    {
        dpp::guild* g = dpp::find_guild(event.command.guild_id);
        bool memberIsInVoice = g->connect_member_voice(event.command.get_issuing_user().id);
        if (!memberIsInVoice)
        {
            event.reply("노래를 재생할 음성 채팅방에 먼저 참가하고 신청해야 합니다!");
            return;
        }
    }

    std::string Query = std::get<std::string>(event.get_parameter("query"));

    event.thinking();

    event.from->log(dpp::loglevel::ll_debug, "음악 ID 쿼리: " + Query);

    std::string musicIDs = getResultFromCommand(("python3 youtube-search.py \"" + Query + "\" & wait").c_str());

    if (!musicIDs.length())
    {
        event.from->log(dpp::loglevel::ll_debug, "유튜브 검색 결과 없음");
        event.edit_response("검색 결과가 없습니다.");
        return;
    }
    
    std::stringstream sstream(musicIDs);
    std::string musicID;
    while (getline(sstream, musicID, '\n')) {
        event.from->log(dpp::loglevel::ll_debug, "musicID: " + musicID);
            event.from->log(dpp::loglevel::ll_debug, "DB쿼리 시도..");
        std::shared_ptr<dpp::embed> embed = Bot->findEmbed(musicID);

        if (embed == nullptr) {
            event.from->log(dpp::loglevel::ll_debug, "DB쿼리 실패");
            event.from->log(dpp::loglevel::ll_debug, "다운로드 시작");
            std::system(("python3 yt-download.py \"" + musicID + "\" & wait").c_str());

            event.from->log(dpp::loglevel::ll_debug, "musicID: " + musicID);
            std::ifstream infofile;
            infofile.open((std::string("Music/") + musicID + ".info.json").c_str());
            event.from->log(dpp::loglevel::ll_debug, std::string("json file name: ") + "Music/" + musicID + ".info.json");
            json document;
            infofile >> document;
            infofile.close();

            embed = Bot->makeEmbed(
                document["webpage_url"],
                document["title"],
                document["uploader"],
                document["id"],
                document["thumbnail"],
                int(document["duration"]));
            
            on_DLCompleted(musicID, *embed, event);
        }
        else {
            event.from->log(dpp::loglevel::ll_debug, "DB쿼리 완료");
            on_DLCompleted(musicID, *embed, event);
        }
    }
    return;
}

void commands::Play::on_DLCompleted(std::string musicID, dpp::embed embed, const dpp::slashcommand_t& event)
{
    static std::string raw_event;
    if (raw_event != event.raw_event){
        raw_event = event.raw_event;

        dpp::message msg(event.command.channel_id, "큐에 다음 곡을 추가했습니다:");
        msg.add_embed(embed);
        event.edit_response(msg);
    }
    else {
        dpp::message followMsg(event.command.channel_id, "");

        dpp::embed followEmbed = dpp::embed()
            .add_field(
                embed.title,
                embed.description,
                true
            )
            .add_field(
                "",
                ""
            );

        followMsg.add_embed(followEmbed);

        event.from->creator->message_create(followMsg);
    }

    auto voiceconn = event.from->get_voice(event.command.guild_id);

    if (!voiceconn || !voiceconn->voiceclient || !voiceconn->voiceclient->is_ready()) {
        event.from->creator->on_voice_ready([this, musicID, embed](const dpp::voice_ready_t& Voice){
            this->Bot->enqueueMusic({musicID, embed}, Voice.voice_client);
        });
    }
    else {
        Bot->enqueueMusic({musicID, embed}, voiceconn->voiceclient);
    }
}