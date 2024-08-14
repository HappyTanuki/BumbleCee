#include <Bot.hpp>
#include <Commands/CommandType.hpp>

IBot::IBot(std::string token, int clusterCount)
{
    botCluster = std::make_shared<dpp::cluster>(token, dpp::i_default_intents, clusterCount);

    botCluster->on_log(logger());
    botCluster->on_slashcommand([&](const dpp::slashcommand_t& event){onCommand(event);});
    botCluster->on_ready([&](const dpp::ready_t &event){onReady(event);});
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
            botCluster->global_command_create(alias);
    
    botCluster->log(dpp::loglevel::ll_info, "Command added to all clusters.");
}

void IBot::start()
{
    botCluster->start(dpp::st_wait);
}