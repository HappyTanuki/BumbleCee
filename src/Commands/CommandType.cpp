#include <Commands/CommandType.hpp>

commands::ICommand::ICommand(std::shared_ptr<dpp::cluster> botCluster)
{
    this->botCluster = botCluster;
}

std::shared_ptr<MusicQueue> commands::VCCommand::getQueue(const dpp::slashcommand_t& event) {
    FMusicQueueID queueID;
    queueID.guild_id = event.command.guild_id;
    queueID.shard_id = event.from->shard_id;

    (*queueMap)[queueID.guild_id] = std::make_shared<MusicQueue>(queueID, botCluster);
    
    return queueMap->find(queueID.guild_id)->second;
}