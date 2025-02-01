#include <Commands/BumbleBeeCommand.hpp>

namespace bumbleBee::commands {
    void Queue::execute(const dpp::slashcommand_t &event) {
        auto queue = musicManager->getQueue(event.command.guild_id);
        auto nowplaying = musicManager->getNowPlaying(event.command.guild_id);

        dpp::message msg;
        dpp::embed embed;

        if (queue.size() == 0) {
            embed
                .set_title("큐가 비었습니다!")
                .set_timestamp(time(0));
            msg.add_embed(embed);
            event.edit_original_response(msg);
            return;
        }
        msg.content = "지금 재생 중:";
        msg.add_embed(nowplaying.embed);

        for (auto iter = queue.begin(); iter != queue.end(); iter++) {
            dpp::message followMsg(event.command.channel_id, "");

            followMsg.add_embed(iter->embed);
            event.from->creator->message_create(followMsg); // 어차피 원래 메시지를 지정해서 수정할 것이기 때문에 먼저 팔로잉 메시지를 작성해도 상관없음.
        }
        
        event.edit_original_response(msg);
    }

    void Queue::init() {
    }
}