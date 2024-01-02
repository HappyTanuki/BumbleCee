#include <Commands/CommandType.hpp>

commands::ICommand::ICommand(std::shared_ptr<dpp::cluster> botCluster)
{
    this->botCluster = botCluster;
}
