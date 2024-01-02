#include <Bot.hpp>
#include <Commands/CommandType.hpp>

IBot::IBot(std::string token, int totalShard)
{
    botCluster = std::make_shared<dpp::cluster>(token, dpp::i_default_intents, totalShard);
    botCluster->on_log(dpp::utility::cout_logger());

    botCluster->on_slashcommand([this](const dpp::slashcommand_t& event){onCommand(event);});
    botCluster->on_ready([this](const dpp::ready_t &event){onReady(event);});
}

void IBot::onCommand(const dpp::slashcommand_t &event)
{
    auto _event = event;
    for (int i = 0; i < commandsArray.size(); i++) {
        for (auto alias : commandsArray[i]->commandObjectVector) {
            if (event.command.get_command_name() == alias.name) {
                (*commandsArray[i])(_event);
            }
        }
    }
}

void IBot::onReady(const dpp::ready_t &event)
{
    if (!dpp::run_once<struct register_bot_commands>())
        return;

    //BotCluster->global_bulk_command_delete();

    for (int i = 0; i < commandsArray.size(); i++) {
        for (auto Alias : commandsArray[i]->commandObjectVector) {
            botCluster->global_command_create(Alias);
        }
    }
}

void IBot::start()
{
    botCluster->start(dpp::st_wait);
}