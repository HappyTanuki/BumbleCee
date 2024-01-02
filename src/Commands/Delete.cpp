#include <Commands/Delete.hpp>
#include <iostream>

commands::Delete::Delete(std::shared_ptr<dpp::cluster> botCluster, std::unordered_map<dpp::snowflake, std::shared_ptr<MusicQueue>> *queueMap)
    : ICommand(botCluster)
{
    this->queueMap = queueMap;
    dpp::slashcommand Command = dpp::slashcommand("d", "큐의 해당하는 번호의 노래를 지웁니다", botCluster->me.id);

    Command.add_option(
        dpp::command_option(dpp::co_string, "pos", "큐 번호", botCluster->me.id)
    );

    commandObjectVector.push_back(Command);
}

void commands::Delete::operator()(const dpp::slashcommand_t& event)
{
    if (std::holds_alternative<std::monostate>(event.get_parameter("pos"))) {
        event.reply("삭제할 노래의 인덱스가 정확하지 않습니다.");
        return;
    }
    std::string Pos = std::get<std::string>(event.get_parameter("pos"));
    event.thinking();

    auto findResult = queueMap->find(event.command.guild_id);
    if (findResult == queueMap->end())
    {
        FMusicQueueID queueID;
        queueID.guild_id = event.command.guild_id;
        queueID.shard_id = event.from->shard_id;

        (*queueMap)[queueID.guild_id] = std::make_shared<MusicQueue>(queueID);
    }
    std::shared_ptr<MusicQueue> queue = queueMap->find(event.command.guild_id)->second;

    auto PopedElement = queue->pop(atoi(Pos.c_str()));

    dpp::embed embed = PopedElement.embed
        .set_timestamp(time(0));

    dpp::message msg(event.command.channel_id, "다음 항목을 큐에서 삭제했습니다!:");

    if (!atoi(Pos.c_str())) {
        dpp::voiceconn* v = event.from->get_voice(event.command.guild_id);

        if (!v || !v->voiceclient || !v->voiceclient->is_ready()) {
            return;
        }

        v->voiceclient->stop_audio();
    }
    
    msg.add_embed(embed);

    event.edit_original_response(msg);
}