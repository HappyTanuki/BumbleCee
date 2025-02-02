#include <Commands/BumbleBeeCommand.hpp>
#include <Utils/QueuedMusicListEmbedProvider.hpp>

namespace bumbleBee::commands {
    void Queue::execute(const dpp::slashcommand_t &event) {
        auto queue = musicManager->getQueue(event.command.guild_id);

        dpp::message msg;
        dpp::embed embed;

        auto queued = QueuedMusicListEmbedProvider::makeEmbed(queue.first, queue.second, musicManager->getRepeat(event.command.guild_id));

        // if (queue.first.size() == 0) {
        //     msg.add_embed(queued.front());
        //     event.edit_original_response(msg);
        //     return;
        // }

        //  &&
        // queue.first.size() != 0 &&
        // *queue.second != nullptr &&
        // (*queue.second)->id != ""

        if (queue.first->size() != 0 && queue.first->end() != queue.second && (*queue.second)->id != "") {
            msg.content = "지금 재생 중:";
            msg.add_embed((*queue.second)->embed);
            event.edit_original_response(msg);
        }
        else {
            msg.content = "재생 중인 노래가 없습니다";
            event.edit_original_response(msg);
        }

        std::thread t([](std::queue<dpp::embed> queued, dpp::snowflake channel_id, dpp::cluster* cluster) {
            std::mutex messageorder;
            std::unique_lock lock(messageorder);
            std::condition_variable messageSentCondition;
            bool messagesent = false;
            if (!queued.empty()) {
                dpp::message followMsg(channel_id, "현재 큐에 있는 항목:");

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
                dpp::message followMsg(channel_id, "");

                followMsg.add_embed(queued.front());

                messagesent = false;
                cluster->message_create(followMsg, [&](const dpp::confirmation_callback_t &callback){
                    messagesent = true;
                    messageSentCondition.notify_all();
                });

                messageSentCondition.wait(lock, [&](){ return messagesent; });
                queued.pop();
            }
        }, queued, event.command.channel_id, event.from->creator);
        t.detach();
    }

    void Queue::init() {
    }
}