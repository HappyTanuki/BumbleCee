#include <Commands/BumbleBeeCommand.hpp>
#include <Utils/ConsoleUtils.hpp>
#include <Settings/SettingsManager.hpp>
#include <dpp/nlohmann/json.hpp>
#include <Utils/QueuedMusicListEmbedProvider.hpp>
#include <Utils/ConsoleUtils.hpp>
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
        // query = "\"" + query + "\"";

        std::queue<std::string> ids =
            ConsoleUtils::safe_execute_command(
            SettingsManager::getYTDLP_CMD(), {
            "--default-search",
            "ytsearch",
            "--flat-playlist",
            "--skip-download",
            "--quiet",
            "--ignore-errors",
            "--print",
            "id",
            query});

        std::queue<std::shared_ptr<MusicQueueElement>> musics;

        int initialQueuedCount = musicManager->size(event.command.guild_id);

        dpp::message msg;

        if (musicManager->size(event.command.guild_id) == 0)
            msg.content = "다음 곡을 재생합니다:";
        else
            msg.content = "큐에 다음 곡을 추가했습니다:";

        if (!ids.empty()) {
            if (ids.front() == "") {
                ids.pop();
            }

            std::string jsonData = ConsoleUtils::safe_execute_command(SettingsManager::getYTDLP_CMD(), {
                "--default-search",
                "ytsearch",
                "--flat-playlist",
                "--skip-download",
                "--quiet",
                "--ignore-errors",
                "-J",
                "http://youtu.be/" + ids.front()
            }).front();

            std::istringstream iss(jsonData);
            nlohmann::json videoDataJson;
            iss >> videoDataJson;

            time_t SongLength = int(videoDataJson["duration"]);
            char SongLengthStr[13];
            tm t;
            t.tm_mday = SongLength / 86400;
            t.tm_hour = (SongLength % 86400)/3600;
            t.tm_min = (SongLength % 3600)/60;
            t.tm_sec = SongLength%60;
            if (t.tm_mday > 0)
                strftime(SongLengthStr, sizeof(SongLengthStr), "%d:%H:%M:%S", &t);
            else if (t.tm_hour > 0)
                strftime(SongLengthStr, sizeof(SongLengthStr), "%H:%M:%S", &t);
            else
                strftime(SongLengthStr, sizeof(SongLengthStr), "%M:%S", &t);

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

        if (!ids.empty()) {
            std::thread t([](
                std::queue<std::string> ids,
                dpp::snowflake guildId,
                dpp::snowflake channelId,
                std::string query,
                dpp::user user,
                dpp::cluster* cluster,
                std::shared_ptr<MusicPlayManager> manager
                ) {
                    while (!ids.empty()) {
                        if (ids.front() == "") {
                            ids.pop();
                            continue;
                        }

                        std::string jsonData = ConsoleUtils::safe_execute_command(SettingsManager::getYTDLP_CMD(), {
                            "--default-search",
                            "ytsearch",
                            "--flat-playlist",
                            "--skip-download",
                            "--quiet",
                            "--ignore-errors",
                            "-J",
                            "http://youtu.be/" + ids.front()
                        }).front();
            
                        std::istringstream iss(jsonData);
                        nlohmann::json videoDataJson;
                        iss >> videoDataJson;

                        time_t SongLength = int(videoDataJson["duration"]);
                        char SongLengthStr[13];
                        tm t;
                        t.tm_mday = SongLength / 86400;
                        t.tm_hour = (SongLength % 86400)/3600;
                        t.tm_min = (SongLength % 3600)/60;
                        t.tm_sec = SongLength%60;
                        if (t.tm_mday > 0)
                            strftime(SongLengthStr, sizeof(SongLengthStr), "%d:%H:%M:%S", &t);
                        else if (t.tm_hour > 0)
                            strftime(SongLengthStr, sizeof(SongLengthStr), "%H:%M:%S", &t);
                        else
                            strftime(SongLengthStr, sizeof(SongLengthStr), "%M:%S", &t);

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

                        auto music = std::make_shared<MusicQueueElement>(ids.front(), query, user, embed);

                        cluster->log(dpp::ll_info, "Enqueuing " + music->embed.title + " - " + music->id);
                        manager->queue_music(guildId, music);
                        ids.pop();
                    }

                    std::mutex messageorder;
                    std::unique_lock lock(messageorder);
                    std::condition_variable messageSentCondition;                   // 개씨발 코드 개더러워 ;;
                    bool messagesent = false;
                    auto queue = manager->getQueue(guildId);
                    auto queued = QueuedMusicListEmbedProvider::makeEmbed(queue.first, queue.second, manager->getRepeat(guildId));
                    if (!queued.empty()) {
                        dpp::message followMsg(channelId, "현재 큐에 있는 항목:");

                        followMsg.add_embed(queued.front());
                        messagesent = false;
                        cluster->message_create(followMsg, [&](const dpp::confirmation_callback_t &callback){
                            messagesent = true;
                            messageSentCondition.notify_all();
                        });

                        messageSentCondition.wait(lock, [&](){ return messagesent; });
                        queued.pop();
                    }
                    while (!queued.empty()) {
                        dpp::message followMsg(channelId, "");

                        followMsg.add_embed(queued.front());
                        messagesent = false;
                        cluster->message_create(followMsg, [&](const dpp::confirmation_callback_t &callback){
                            messagesent = true;
                            messageSentCondition.notify_all();
                        });

                        messageSentCondition.wait(lock, [&](){ return messagesent; });
                        queued.pop();
                    }
                },
                ids,
                event.command.guild_id,
                event.command.channel_id,
                query,
                event.command.usr,
                event.from->creator,
                musicManager);

            t.detach();
        }
        
        if (!musics.empty()) {
            event.from->creator->log(dpp::ll_info, "Enqueuing " + musics.front()->embed.title + " - " + musics.front()->id);
            musicManager->queue_music(event.command.guild_id, musics.front());
            msg.add_embed(musics.front()->embed);
            musics.pop();

            event.edit_original_response(msg);
            musicManager->queuedCondition.notify_all();
        }
        else {
            msg.content = "검색 결과가 없습니다";
            event.edit_original_response(msg);
        }

        if (musicManager->getNowPlaying(event.command.guild_id).id == "") {
            dpp::voiceconn* v = event.from->get_voice(event.command.guild_id);

            if (!v || !v->voiceclient || !v->voiceclient->is_ready()) {
                event.edit_original_response(dpp::message("현재 음성 채팅방에 있는 상태가 아닙니다!"));
                return;
            }
            v->voiceclient->insert_marker();
            v->voiceclient->pause_audio(false);
        }
    }

    void Play::init() {
        add_option(dpp::command_option(dpp::co_string, "query", "링크 또는 검색어", true));
    }
}