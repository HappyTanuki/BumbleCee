#include <Commands/Queue.hpp>
#include <iostream>
#include <sstream>
#include <cmath>

namespace commands {
dpp::embed makeEmbed(std::list<FQueueElement>::iterator& iter, std::list<FQueueElement>::iterator end, bool Repeat = false, int Index = 0)
{
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
        Number << Index;
        embed.add_field(
            Number.str(),
            "",
            true
        )
        .add_field(
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
}

commands::Queue::Queue(std::shared_ptr<dpp::cluster> botCluster, std::unordered_map<dpp::snowflake, std::shared_ptr<MusicQueue>> *queueMap)
    : ICommand(botCluster)
{
    this->queueMap = queueMap;
    dpp::slashcommand command = dpp::slashcommand("q", "노래 예약 큐 확인", botCluster->me.id);

    commandObjectVector.push_back(command);
}

void commands::Queue::operator()(const dpp::slashcommand_t& event) {
    dpp::message msg(event.command.channel_id, "지금 재생 중:");

    auto findResult = queueMap->find(event.command.guild_id);
    if (findResult == queueMap->end())
    {
        FMusicQueueID queueID;
        queueID.guild_id = event.command.guild_id;
        queueID.shard_id = event.from->shard_id;

        (*queueMap)[queueID.guild_id] = std::make_shared<MusicQueue>(queueID);
    }
    std::shared_ptr<MusicQueue> queue = queueMap->find(event.command.guild_id)->second;

    msg.add_embed(queue->peek(0).embed);

    event.reply(msg, [this, queue, event](const dpp::confirmation_callback_t &_event) {
        auto iter = queue->begin();
        iter++;
        for (int i = 0; i < ceil((queue->size() - 1) / 5.0); i++) {
            dpp::embed followEmbed = makeEmbed(iter, queue->end(), queue->repeat, i * 5 + 1);

            dpp::message followMsg;
            followMsg.channel_id = event.command.channel_id;

            if (i == 0) {
                followMsg.content = "현재 큐에 있는 항목:";
            }
            followMsg.add_embed(followEmbed);

            botCluster->message_create(followMsg);
        }
    });
}