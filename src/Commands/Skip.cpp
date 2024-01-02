#include <Commands/Skip.hpp>
#include <dpp/dpp.h>
#include <string>

commands::Skip::Skip(std::shared_ptr<dpp::cluster> botCluster, std::unordered_map<dpp::snowflake, std::shared_ptr<MusicQueue>> *queueMap)
    : ICommand(botCluster)
{
    this->queueMap = queueMap;
    dpp::slashcommand command = dpp::slashcommand("s", "현재곡 스킵", botCluster->me.id);

    commandObjectVector.push_back(command);
}

void commands::Skip::operator()(const dpp::slashcommand_t& event) {
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
    
    queue->play(botCluster);

    event.reply("스킵했습니다!");

    return;
}