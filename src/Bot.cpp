#include <Bot.hpp>
#include <Commands/CommandType.hpp>

IBot::IBot(std::string token, int clusterCount, std::string DBURL, std::string DBID, std::string DBPassword)
{
    this->DBURL = std::make_shared<sql::SQLString>(DBURL);
    sql::Properties pro({
        {"user", DBID},
        {"password", DBPassword}
    });
    this->DBProperties = std::make_shared<sql::Properties>(pro);
    DBDriver = sql::mariadb::get_driver_instance();

    for (int i = 0; i<clusterCount; i++)
    {
        std::shared_ptr<dpp::cluster> cluster = std::make_shared<dpp::cluster>(token, dpp::i_default_intents);

        cluster->on_log(dpp::utility::cout_logger());
        cluster->on_slashcommand([&](const dpp::slashcommand_t& event){onCommand(event);});
        cluster->on_ready([&](const dpp::ready_t &event){onReady(event);});
        botClusters.push_back(cluster);
    }
}

void IBot::onCommand(const dpp::slashcommand_t &event)
{
    auto _event = event;
    for (auto command : commandsArray)
        for (auto alias : command->commandObjectVector)
            if (event.command.get_command_name() == alias.name)
                (*command)(_event);
}

void IBot::onReady(const dpp::ready_t &event)
{
    if (!dpp::run_once<struct register_bot_commands>())
        return;

    //BotCluster->global_bulk_command_delete();

    for (auto command : commandsArray)
        for (auto alias : command->commandObjectVector)
            for (auto cluster : botClusters)
                cluster->global_command_create(alias);
    
    botClusters[0]->log(dpp::loglevel::ll_info, "Command added to all clusters.");
}

void IBot::start()
{
    if (botClusters.size() == 1)
    {
        botClusters[0]->start(dpp::st_wait);
        return;
    }

    for (int i = 0; i < botClusters.size() - 1; i++)
    {
        botClusters[i]->start(dpp::st_return);
    }
    botClusters[botClusters.size() - 1]->start(dpp::st_wait);
}