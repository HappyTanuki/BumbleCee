#include "Bot.hpp"

IBot::IBot(std::string Token) {
    bot = std::make_shared<dpp::cluster>(Token);
    bot->on_log(dpp::utility::cout_logger());

    bot->on_slashcommand([this](const dpp::slashcommand_t& Event) {OnCommand(Event);});
    bot->on_ready([this](const dpp::ready_t& Event) {OnReady(Event);});
}

void IBot::AddCommand(ICommand &Command) {
    CommandsArray.push_back(&Command);
}

void IBot::Start() {
    bot->start(dpp::st_wait);
}

void IBot::OnReady(const dpp::ready_t& Event) {
    if (!dpp::run_once<struct register_bot_commands>())
        return;

    bot->global_bulk_command_delete();

    for (auto command : CommandsArray) {
        for (auto Alias : command->CommandObjectVector) {
            bot->global_command_create(Alias);
        }
    }
}

void IBot::OnCommand(const dpp::slashcommand_t& Event) {
    for (auto Command : CommandsArray) {
        for (auto Alias : Command->CommandObjectVector) {
            if (Event.command.get_command_name() == Alias.name) {
                (*Command)(Event);
            }
        }
    }
}