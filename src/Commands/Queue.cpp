#include <Commands/Queue.hpp>
#include <iostream>

namespace Commands {
    Queue::Queue(std::shared_ptr<BumbleCeepp> Bot) {
        this->Bot = Bot;

        dpp::slashcommand Command = dpp::slashcommand("q", "노래 예약 큐 확인", Bot->BotCluster->me.id);

        CommandObjectVector.push_back(Command);
    }

    void Queue::operator()(std::list<FQueueElement>& MusicQueue, const dpp::slashcommand_t& Event) {
        if (MusicQueue.empty()) {
            dpp::embed embed = dpp::embed()
                .set_title("큐가 비었습니다!")
                .set_color(dpp::colors::sti_blue)
                .set_timestamp(time(0));

            if (Bot->Repeat)
                embed.add_field(":repeat:","");

            dpp::message msg(Event.command.channel_id, "현재 큐에 있는 항목:");
            msg.add_embed(embed);

            Event.reply(msg);
            return;
        }

        dpp::embed embed = dpp::embed()
            .set_title("큐 항목:")
            .set_color(dpp::colors::sti_blue)
            .set_timestamp(time(0));

        int i = 0;
        char number[30] = {0, };

        for (auto iter = MusicQueue.begin(); iter != MusicQueue.end(); iter++) {
            if (!i) {
                embed.add_field(
                    "현재 재생 중",
                    "",
                    true
                );
            }
            else {
                sprintf(number, "%d", i);
                embed.add_field(
                    number,
                    "",
                    true
                );
            }

            embed.add_field(
                iter->title,
                iter->description,
                true
            )
            .add_field(
                "",
                ""
            );

            i++;
        }

        if (Bot->Repeat)
            embed.add_field(":repeat:","");

        dpp::message msg(Event.command.channel_id, "현재 큐에 있는 항목:");
        msg.add_embed(embed);

        Event.reply(msg);
    }
}