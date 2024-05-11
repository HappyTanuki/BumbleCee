#include <Commands/Skip.hpp>
#include <dpp/dpp.h>
#include <string>

commands::Skip::Skip(dpp::snowflake botID, BumbleCeepp* Bot)
 : ICommand(botID, Bot)
{
    dpp::slashcommand command = dpp::slashcommand("s", "현재곡 스킵", botID);

    commandObjectVector.push_back(command);
}

void commands::Skip::operator()(const dpp::slashcommand_t& event) {
    dpp::voiceconn* v = event.from->get_voice(event.command.guild_id);

    if (!v || !v->voiceclient || !v->voiceclient->is_ready()) {
        return;
    }
    v->voiceclient->skip_to_next_marker();

    event.reply("스킵했습니다!");

    return;
}