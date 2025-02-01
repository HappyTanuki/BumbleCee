#include <Commands/BumbleBeeCommand.hpp>
#include <Utils/ConsoleUtils.hpp>
#include <Settings/SettingsManager.hpp>
#include <dpp/nlohmann/json.hpp>
#include <variant>

namespace bumbleBee::commands {
    void Play::execute(const dpp::slashcommand_t &event) {
        dpp::guild *g = dpp::find_guild(event.command.guild_id);

        if (!g) { //wtf?
            event.edit_original_response(dpp::message("GUILD NOT FOUND!! WHAT IS THIS SORCERY??"));
            return;
        }
        if (std::holds_alternative<std::monostate>(event.get_parameter("query"))) // 이거 필요한가???
        {
            event.reply("노래를 재생하려면 검색어 또는 링크를 입력해 주십시오.");
            return;
        }
        if (!event.from->get_voice(event.command.guild_id) && !g->connect_member_voice(event.command.usr.id)) {
            event.edit_original_response(dpp::message("노래를 재생할 음성 채팅방에 먼저 참가하고 신청해야 합니다!"));
            return;
        }
        std::string query = std::get<std::string>(event.get_parameter("query"));

        std::queue<std::string> ids =
            ConsoleUtils::getResultFromCommand(
            SettingsManager::getYTDLP_CMD() +
            " --default-search ytsearch --flat-playlist --skip-download --quiet --ignore-errors --print id " + query);

        std::queue<std::shared_ptr<MusicQueueElement>> musics;

        int initialQueuedCount = musicManager->size(event.command.guild_id);

        dpp::message msg;

        if (musicManager->size(event.command.guild_id) == 0)
            msg.content = "다음 곡을 재생합니다:";
        else
            msg.content = "큐에 다음 곡을 추가했습니다:";

        while (!ids.empty()) {
            if (ids.front() == "") {
                ids.pop();
                continue;
            }

            FILE* file = popen((SettingsManager::getYTDLP_CMD() +
                " --default-search ytsearch --flat-playlist --skip-download --quiet --ignore-errors -J http://youtu.be/" + ids.front()).c_str(), "r");
        
            std::ostringstream oss;
            char buffer[1024];
            size_t bytesRead;
            
            while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0) {
                oss.write(buffer, bytesRead);
            }
            fclose(file);

            std::istringstream iss(oss.str());
            nlohmann::json videoDataJson;
            iss >> videoDataJson;

            time_t SongLength = int(videoDataJson["duration"]);
            char SongLengthStr[10];
            tm t;
            t.tm_mday = SongLength / 86400;
            t.tm_hour = (SongLength % 86400)/3600;
            t.tm_min = (SongLength % 3600)/60;
            t.tm_sec = SongLength%60;
            strftime(SongLengthStr, sizeof(SongLengthStr), "%X", &t);

            dpp::embed embed = dpp::embed()
                .set_color(dpp::colors::sti_blue)
                .set_title(std::string(videoDataJson["title"]))
                .set_description(std::string(videoDataJson["uploader"]))
                .set_url(std::string(videoDataJson["webpage_url"]))
                .set_image(std::string(videoDataJson["thumbnail"]))
                .add_field(
                    "길이",
                    SongLengthStr,
                    true
                );

            musics.push(std::make_shared<MusicQueueElement>(ids.front(), query, event.command.usr, embed));
            ids.pop();
        }

        event.from->creator->log(dpp::ll_info, "Enqueuing " + musics.front()->embed.title + " - " + musics.front()->id);
        musicManager->queue_music(event.command.guild_id, musics.front());
        msg.add_embed(musics.front()->embed);
        musics.pop();

        event.edit_original_response(msg);
        musicManager->queuedCondition.notify_all();

        while (!musics.empty()) {
            event.from->creator->log(dpp::ll_info, "Enqueuing " + musics.front()->embed.title + " - " + musics.front()->id);
            dpp::message followMsg(event.command.channel_id, "");

            followMsg.add_embed(musics.front()->embed);
            event.from->creator->message_create(followMsg); // 어차피 원래 메시지를 지정해서 수정할 것이기 때문에 먼저 팔로잉 메시지를 작성해도 상관없음.

            musicManager->queue_music(event.command.guild_id, musics.front());
            musics.pop();
        }
    }

    void Play::init() {
        add_option(dpp::command_option(dpp::co_string, "query", "링크 또는 검색어", true));
    }
}