#include "BumbleCeepp.hpp"
#include <string>
#include <FQueueElement.hpp>
#include <Commands/Commands.hpp>

BumbleCeepp::BumbleCeepp(std::string token, int totalShard)
    : IBot(token, totalShard)
{
    commandsArray.push_back(std::make_shared<commands::Play>(botCluster, &queueMap));
    commandsArray.push_back(std::make_shared<commands::Repeat>(botCluster, &queueMap));
    commandsArray.push_back(std::make_shared<commands::Queue>(botCluster, &queueMap));
    commandsArray.push_back(std::make_shared<commands::Skip>(botCluster, &queueMap));
    commandsArray.push_back(std::make_shared<commands::Leave>(botCluster, &queueMap));
    commandsArray.push_back(std::make_shared<commands::Delete>(botCluster, &queueMap));

    std::cout << "Command added.\n";
}