#include "BumbleCeepp.hpp"
#include <string>

BumbleCeepp::BumbleCeepp(std::string Token) : IBot(Token) {
}

void BumbleCeepp::OnCommand(const dpp::slashcommand_t& Event) {
    for (auto Command : CommandsArray) {
        for (auto Alias : Command->CommandObjectVector) {
            if (Event.command.get_command_name() == Alias.name) {
                (*Command)(MusicQueue, Event);
            }
        }
    }
}