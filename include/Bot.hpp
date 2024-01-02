#pragma once
#include <Commands/CommandType.hpp>
#include <dpp/dpp.h>
#include <memory>

class IBot {
public:
    IBot(std::string token, int totalShard);
    void start();
    void onCommand(const dpp::slashcommand_t &event);
    void onReady(const dpp::ready_t &event);

    std::shared_ptr<dpp::cluster> botCluster;
    std::vector<std::shared_ptr<commands::ICommand>> commandsArray;
protected:
};
