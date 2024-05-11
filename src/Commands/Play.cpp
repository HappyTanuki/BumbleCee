#include <Commands/Play.hpp>
#include <dpp/dpp.h>
#include <dpp/nlohmann/json.hpp>
#include <string>
#include <filesystem>
#include <ctime>

using json = nlohmann::json;

commands::Play::Play(std::shared_ptr<dpp::cluster> botCluster, std::unordered_map<dpp::snowflake, std::shared_ptr<MusicQueue>> *queueMap)
    : VCCommand(botCluster)
{
    this->queueMap = queueMap;
    dpp::slashcommand command = dpp::slashcommand("p", "노래 재생", botCluster->me.id);

    command.add_option(
        dpp::command_option(dpp::co_string, "query", "링크 또는 검색어", botCluster->me.id)
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
        if (!dpp::find_guild(event.command.guild_id)->connect_member_voice(event.command.get_issuing_user().id))
        {
            return event.reply("노래를 재생할 음성 채팅방에 먼저 참가하고 신청해야 합니다!");
        }
    }

    std::string Query = std::get<std::string>(event.get_parameter("query"));

    event.thinking();

    std::shared_ptr<MusicQueue> queue = getQueue(event);

    std::cout << "다운로드 시작" << "\n";
    std::system(("python3 yt-download.py \"" + Query + "\" & wait").c_str());
    std::cout << "다운로드 완료" << "\n";

    dpp::message msg(event.command.channel_id, "큐에 다음 곡을 추가했습니다:");

    std::ifstream infofile, idfile;
    json document;
    std::string ID;
    std::queue<FQueueElement> RequestedMusic;
    idfile.open("Temp/CurMusic");
    while (std::getline(idfile, ID))
    {
        std::cout << ID << "\n";
        infofile.open("Music/" + ID + ".info.json");
        infofile >> document;
        infofile.close();

        time_t SongLength = int(document["duration"]);
        char SongLengthStr[10];
        tm t;
        t.tm_mday = SongLength / 86400;
        t.tm_hour = (SongLength % 86400)/3600;
        t.tm_min = (SongLength % 3600)/60;
        t.tm_sec = SongLength%60;
        strftime(SongLengthStr, sizeof(SongLengthStr), "%X", &t);

        FQueueElement Data = {
            std::string(document["webpage_url"]),
            std::string(document["title"]),
            std::string(document["uploader"]),
            std::string(document["id"]),
            std::string(document["thumbnail"]),
            to_string(document["duration"]),
            dpp::embed()
                .set_color(dpp::colors::sti_blue)
                .set_title(Data.title)
                .set_description(Data.description)
                .set_url(Data.URL)
                .set_image(Data.thumbnail)
                .add_field(
                    "길이",
                    SongLengthStr,
                    true
                )
        };

        (*queue) += Data;

        RequestedMusic.push(Data);
    }
    idfile.close();
    std::system("rm -f Temp/CurMusic");
    std::cout << "queued\n";

    msg.add_embed(RequestedMusic.front().embed);
    RequestedMusic.pop();
    event.edit_original_response(msg);

    while (!RequestedMusic.empty())
    {
        dpp::message followMsg(event.command.channel_id, "");

        followMsg.add_embed(RequestedMusic.front().embed);
        RequestedMusic.pop();

        botCluster->message_create(followMsg);
    }
    std::cout << "replied\n";

    dpp::voiceconn* v = event.from->get_voice(event.command.guild_id);

    /* If the voice channel was invalid, or there is an issue with it, then tell the user. */
    if (v && v->voiceclient && v->voiceclient->is_ready())
    {
        queue->play();
    }

    botCluster->on_voice_ready([this, queue](const dpp::voice_ready_t& Voice){ queue->play(); });
    return;
}