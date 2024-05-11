#include <Commands/Leave.hpp>
#include <iostream>

commands::Leave::Leave(dpp::snowflake botID, BumbleCeepp* Bot)
 : ICommand(botID, Bot)
{
    dpp::slashcommand command = dpp::slashcommand("l", "음챗을 떠납니다", botID);

    commandObjectVector.push_back(command);
}

void commands::Leave::operator()(const dpp::slashcommand_t& event)
{
    dpp::voiceconn* v = event.from->get_voice(event.command.guild_id);

    if (!v || !v->voiceclient || !v->voiceclient->is_ready()) {
        return;
    }
    v->voiceclient->stop_audio();
    event.from->disconnect_voice(event.command.guild_id);

    dpp::message msg(event.command.channel_id, "음성 채팅방을 떠납니다!");

    event.reply(msg);
}