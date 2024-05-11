#pragma once
#include <Commands/CommandType.hpp>
#include <dpp/dpp.h>
#include <memory>
#include <mariadb/conncpp.hpp>

class IBot {
public:
    IBot(std::string token, int clusterCount, std::string DBURL, std::string DBID, std::string DBPassword);
    virtual void start();
    virtual void onCommand(const dpp::slashcommand_t &event);
    virtual void onReady(const dpp::ready_t &event);

    std::vector<std::shared_ptr<dpp::cluster>> botClusters;
    std::vector<std::shared_ptr<commands::ICommand>> commandsArray;

protected:
    sql::Driver* DBDriver;
    std::shared_ptr<sql::SQLString> DBURL;
    std::shared_ptr<sql::Properties> DBProperties;
};
