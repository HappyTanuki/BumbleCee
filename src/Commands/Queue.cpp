#include <Commands/Queue.hpp>
#include <iostream>
#include <sstream>
#include <cmath>

namespace Commands {
    dpp::embed MakeEmbed(std::list<FQueueElement>::iterator& iter, std::list<FQueueElement>::iterator end, bool Repeat = false, int Index = 0) {
        dpp::embed embed = dpp::embed()
            .set_color(dpp::colors::sti_blue);

        if (iter == end) {
            embed
                .set_title("큐가 비었습니다!")
                .set_timestamp(time(0));

            if (Repeat)
                embed.add_field(":repeat:","");

            return embed;
        }

        std::ostringstream Number;
        int Start = Index;

        for (; (Index < Start + 5) && (iter != end); iter++, Index++) {
            Number.clear();
            Number.str("");
            if (!Index) {
                embed.add_field(
                    "현재 재생 중",
                    "",
                    true
                );
            }
            else {
                Number << Index;
                embed.add_field(
                    Number.str(),
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
        }

        if (iter == end) {
            embed.set_timestamp(time(0));
            if (Repeat)
                embed.add_field(":repeat:","");
        }

        return embed;
    }

    Queue::Queue(std::shared_ptr<BumbleCeepp> Bot) {
        this->Bot = Bot;

        dpp::slashcommand Command = dpp::slashcommand("q", "노래 예약 큐 확인", Bot->BotCluster->me.id);

        CommandObjectVector.push_back(Command);
    }

    void Queue::operator()(std::list<FQueueElement>& MusicQueue, const dpp::slashcommand_t& Event) {
        auto iter = MusicQueue.begin();
        dpp::embed embed = MakeEmbed(iter, MusicQueue.end(), Bot->Repeat, 0);
        dpp::message msg(Event.command.channel_id, "현재 큐에 있는 항목:");
        
        msg.add_embed(embed);

        Event.reply(msg, [this, Event](const dpp::confirmation_callback_t &_Event) {
            auto _iter = Bot->MusicQueue.begin();
            std::advance(_iter, 5);
            for (int i = 0; i < ceil(Bot->MusicQueue.size() / 5.0) - 1; i++) {
                dpp::embed FollowEmbed = MakeEmbed(_iter, Bot->MusicQueue.end(), Bot->Repeat, (i+1)*5);
                dpp::message FollowMsg(Event.command.channel_id, "");
                FollowMsg.add_embed(FollowEmbed);

                Bot->BotCluster->message_create(FollowMsg);
            }
        });
    }
}