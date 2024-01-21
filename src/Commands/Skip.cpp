#include <Commands/Skip.hpp>
#include <dpp/dpp.h>
#include <string>

commands::Skip::Skip(std::shared_ptr<dpp::cluster> botCluster, std::unordered_map<dpp::snowflake, std::shared_ptr<MusicQueue>> *queueMap)
    : VCCommand(botCluster)
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
    v->voiceclient->insert_marker("next marker");

    std::shared_ptr<MusicQueue> queue = getQueue(event);

    event.reply("스킵했습니다!");

    return;
}