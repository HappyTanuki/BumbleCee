#include <Commands/Queue.hpp>
#include <iostream>

namespace Commands {
    Queue::Queue(std::shared_ptr<BumbleCeepp> Bot) {
        this->Bot = Bot;

        dpp::slashcommand Command = dpp::slashcommand("queue", "노래 예약 큐 확인", Bot->BotCluster->me.id);
        dpp::slashcommand Alias = dpp::slashcommand("q", "노래 예약 큐 확인", Bot->BotCluster->me.id);

        CommandObjectVector.push_back(Command);
        CommandObjectVector.push_back(Alias);
    }

    void Queue::operator()(std::list<FQueueElement>& MusicQueue, const dpp::slashcommand_t& Event) {
        dpp::embed embed = dpp::embed()
            .set_title("큐 항목:")
            .set_color(dpp::colors::sti_blue)
            .set_timestamp(time(0));

        for (auto iter = MusicQueue.begin(); iter != MusicQueue.end(); iter++) {
            embed.add_field(
                iter->title,
                iter->description
            );
        }

        dpp::message msg(Event.command.channel_id, "현재 큐에 있는 항목:");
        msg.add_embed(embed);

        Event.reply(msg);
    }
}