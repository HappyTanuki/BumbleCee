#include <Commands/BumbleBeeCommand.hpp>
#include <Utils/ConsoleUtils.hpp>
#include <variant>

namespace bumbleBee::commands {
    void Play::execute(const dpp::slashcommand_t &event) {
        dpp::guild *g = dpp::find_guild(event.command.guild_id);

        if (!g) { //wtf?
            event.edit_original_response(dpp::message("GUILD NOT FOUND!! WHAT IS THIS SORCERY??"));
            return;
        }
        if (std::holds_alternative<std::monostate>(event.get_parameter("query")))
        {
            event.reply("노래를 재생하려면 검색어 또는 링크를 입력해 주십시오.");
            return;
        }
        if (!event.from->get_voice(event.command.guild_id) && !g->connect_member_voice(event.command.usr.id)) {
            event.edit_original_response(dpp::message("노래를 재생할 음성 채팅방에 먼저 참가하고 신청해야 합니다!"));
            return;
        }
        std::string query = std::get<std::string>(event.get_parameter("query"));

        std::queue<std::string> ids = ConsoleUtils::getResultFromCommand("./yt-dlp --default-search ytsearch --flat-playlist --skip-download --quiet --ignore-errors --print id " + query);

        std::queue<MusicQueueElement> musics;

        dpp::message origMsg;

        if (ids.size() >= 2) {
            event.from->creator->log(dpp::ll_info, "Playlist detected.");
            while (!ids.empty()) {
                if (ids.front() == "") {
                    ids.pop();
                    continue;
                }
                event.from->creator->log(dpp::ll_info, "Enqueuing " + ids.front());

                MusicQueueElement music(
                    ids.front(),
                    query,
                    event.command.usr
                );

                musics.push(music);
                ids.pop();
            }
        }

        MusicQueueElement music(
            ids.front(),
            query,
            event.command.usr
        );
        musics.push(music);

        while (!musics.empty()) {
            auto element = musics.front();
            musicManager->queue_music(std::make_shared<MusicQueueElement>(element));
            musics.pop();
        }

        event.edit_original_response(dpp::message("play"));
        musicManager->queuedCondition.notify_one();
    }

    void Play::init() {
        add_option(dpp::command_option(dpp::co_string, "query", "링크 또는 검색어", true));
    }
}