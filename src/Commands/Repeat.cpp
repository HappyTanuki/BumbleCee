#include <Commands/Repeat.hpp>
#include <dpp/dpp.h>
#include <string>

commands::Repeat::Repeat(dpp::snowflake botID, BumbleCeepp* Bot)
 : ICommand(botID, Bot)
{
    dpp::slashcommand command = dpp::slashcommand("r", "반복 켜기/끄기", botID);

    commandObjectVector.push_back(command);
}

void commands::Repeat::operator()(const dpp::slashcommand_t& event) {
    
    if (Bot->repeat) {
        event.reply("반복을 껐습니다.");
        Bot->repeat = false;
    }
    else {
        event.reply("반복을 켰습니다.");
        Bot->repeat = true;
    }

    return;
}