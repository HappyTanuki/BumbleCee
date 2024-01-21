#include <Commands/Repeat.hpp>
#include <dpp/dpp.h>
#include <string>

commands::Repeat::Repeat(std::shared_ptr<dpp::cluster> botCluster, std::unordered_map<dpp::snowflake, std::shared_ptr<MusicQueue>> *queueMap)
    : VCCommand(botCluster)
{
    this->queueMap = queueMap;
    dpp::slashcommand command = dpp::slashcommand("r", "반복 켜기/끄기", botCluster->me.id);

    commandObjectVector.push_back(command);
}

void commands::Repeat::operator()(const dpp::slashcommand_t& event) {
    std::shared_ptr<MusicQueue> queue = getQueue(event);

    if (queue->repeat) {
        event.reply("반복을 껐습니다.");
        queue->repeat = false;
    }
    else {
        event.reply("반복을 켰습니다.");
        queue->repeat = true;
    }

    return;
}