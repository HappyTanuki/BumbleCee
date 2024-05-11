#include <Commands/Play.hpp>
#include <dpp/dpp.h>
#include <dpp/nlohmann/json.hpp>
#include <string>
#include <filesystem>
#include <ctime>

using json = nlohmann::json;

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
        if (!dpp::find_guild(event.command.guild_id)->connect_member_voice(event.command.get_issuing_user().id))
        {
            return event.reply("노래를 재생할 음성 채팅방에 먼저 참가하고 신청해야 합니다!");
        }
    }

    std::string Query = std::get<std::string>(event.get_parameter("query"));

    event.thinking();

    event.from->log(dpp::loglevel::ll_trace, "음악 다운로드 시작");
    std::system(("python3 yt-download.py \"" + Query + "\" & wait").c_str());
    event.from->log(dpp::loglevel::ll_trace, "음악 다운로드 완료");

    std::ifstream infofile, idfile;
    json document;
    std::string ID;
    std::queue<FQueueElement> RequestedMusic;
    idfile.open("Temp/CurMusic");
    while (std::getline(idfile, ID))
    {
        event.from->log(dpp::loglevel::ll_trace, "Red ID : " + ID);
        infofile.open("Music/" + ID + ".info.json");
        infofile >> document;
        infofile.close();

        FQueueElement Data = {
            ID,
            Bot->makeEmbed(
            document["webpage_url"],
            document["title"],
            document["uploader"],
            document["id"],
            document["thumbnail"],
            int(document["duration"]))
        };

        RequestedMusic.push(Data);
    }
    idfile.close();
    std::system("rm -f Temp/CurMusic");

    dpp::voiceconn* v = event.from->get_voice(event.command.guild_id);

    /* If the voice channel was invalid, or there is an issue with it, then tell the user. */
    if (v && v->voiceclient && v->voiceclient->is_ready())
    {
        auto _copiedQueue = RequestedMusic;
        while (!_copiedQueue.empty())
        {
            Bot->enqueueMusic(_copiedQueue.front(), v->voiceclient);
            _copiedQueue.pop();
        }
    }
    else
    {
        auto _copiedQueue = RequestedMusic;
        event.from->creator->on_voice_ready([&](const dpp::voice_ready_t& Voice)
        {
            while (!_copiedQueue.empty())
            {
                auto item = _copiedQueue.front();
                Bot->enqueueMusic(item, Voice.voice_client);
                _copiedQueue.pop();
            }
        });
    }

    dpp::message msg(event.command.channel_id, "큐에 다음 곡을 추가했습니다:");
    msg.add_embed(RequestedMusic.front().embed);
    RequestedMusic.pop();
    event.edit_original_response(msg);

    while (!RequestedMusic.empty())
    {
        dpp::message followMsg(event.command.channel_id, "");

        followMsg.add_embed(RequestedMusic.front().embed);
        RequestedMusic.pop();

        event.from->creator->message_create(followMsg);
    }

    return;
}