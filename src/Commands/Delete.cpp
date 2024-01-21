#include <Commands/Delete.hpp>
#include <iostream>

commands::Delete::Delete(std::shared_ptr<dpp::cluster> botCluster, std::unordered_map<dpp::snowflake, std::shared_ptr<MusicQueue>> *queueMap)
    : VCCommand(botCluster)
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

    std::shared_ptr<MusicQueue> queue = getQueue(event);

    auto index = atoi(Pos.c_str());

    int queueSize = queue->size();

    std::cout << "queue size : " << queueSize << "\n";

    if (index < 0 || (queueSize - 1) < index) {
        std::cout << "invalid index : " << index << ", " + Pos + "\n";

        event.edit_original_response(dpp::message(event.command.channel_id, "이상한 인덱스 위치. Pos : " + Pos));
        return;
    }

    auto PopedElement = queue->pop(index);

    dpp::embed embed = PopedElement.embed
        .set_timestamp(time(0));

    dpp::message msg(event.command.channel_id, "다음 항목을 큐에서 삭제했습니다!:");

    if (atoi(Pos.c_str()) == 0) {
        dpp::voiceconn* v = event.from->get_voice(event.command.guild_id);

        if (!v || !v->voiceclient || !v->voiceclient->is_ready()) {
            return;
        }

        v->voiceclient->stop_audio();
        v->voiceclient->insert_marker("end of music");
    }
    
    msg.add_embed(embed);

    event.edit_original_response(msg);
}