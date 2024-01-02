#include <Commands/Leave.hpp>
#include <iostream>

commands::Leave::Leave(std::shared_ptr<dpp::cluster> botCluster, std::unordered_map<dpp::snowflake, std::shared_ptr<MusicQueue>> *queueMap)
    : ICommand(botCluster)
{
    this->queueMap = queueMap;
    dpp::slashcommand command = dpp::slashcommand("l", "음챗을 떠납니다", botCluster->me.id);

    commandObjectVector.push_back(command);
}

void commands::Leave::operator()(const dpp::slashcommand_t& event)
{
    dpp::voiceconn* v = event.from->get_voice(event.command.guild_id);

    if (!v || !v->voiceclient || !v->voiceclient->is_ready()) {
        return;
    }
    v->voiceclient->stop_audio();

    auto findResult = queueMap->find(event.command.guild_id);
    if (findResult == queueMap->end())
    {
        FMusicQueueID queueID;
        queueID.guild_id = event.command.guild_id;
        queueID.shard_id = event.from->shard_id;

        (*queueMap)[queueID.guild_id] = std::make_shared<MusicQueue>(queueID);
    }
    std::shared_ptr<MusicQueue> queue = queueMap->find(event.command.guild_id)->second;

    queue->clear();
    event.from->disconnect_voice(event.command.guild_id);

    dpp::message msg(event.command.channel_id, "음성 채팅방을 떠납니다!");

    event.reply(msg);
}