#pragma once
#include <Commands/CommandType.hpp>
#include <dpp/dpp.h>
#include <memory>
#include <mariadb/conncpp.hpp>

class IBot {
public:
    IBot(std::string token, int clusterCount = 0);
    virtual void start();
    virtual void onCommand(const dpp::slashcommand_t &event);
    virtual void onReady(const dpp::ready_t &event);

    std::shared_ptr<dpp::cluster> botCluster;
    std::vector<std::shared_ptr<commands::ICommand>> commandsArray;

    std::function<void(const dpp::log_t&)> logger() {
        return [&](const dpp::log_t& event){
            if (event.severity >= dpp::ll_error)
                std::cerr << "[" << dpp::utility::current_date_time() << "] " << dpp::utility::loglevel(event.severity) << ": " << event.message << std::endl;
            else if (event.severity - logLevel >= 0)
                std::clog << "[" << dpp::utility::current_date_time() << "] " << dpp::utility::loglevel(event.severity) << ": " << event.message << std::endl;
        };
    };

    dpp::loglevel logLevel = dpp::ll_debug;
};
