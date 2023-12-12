#include <Commands/Join.hpp>
#include <iostream>
#include <dpp/dpp.h>

Join::Join(dpp::snowflake Id) {
    dpp::slashcommand Command = dpp::slashcommand("join", "asdf", Id);

    dpp::slashcommand Alias = dpp::slashcommand("j", "asdf", Id);

    CommandObjectVector.push_back(Command);
    CommandObjectVector.push_back(Alias);
}

void Join::operator()(std::list<std::string>& MusicQueue, const dpp::slashcommand_t& Event) {
    dpp::guild* g = dpp::find_guild(Event.command.guild_id);
 
    /* Attempt to connect to a voice channel, returns false if we fail to connect. */
    if (!g->connect_member_voice(Event.command.get_issuing_user().id)) {
        Event.reply("You don't seem to be in a voice channel!");
        return;
    }
    
    /* Tell the user we joined their channel. */
    Event.reply("Joined your channel!");
}