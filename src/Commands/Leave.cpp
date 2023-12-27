#include <Commands/Leave.hpp>
#include <iostream>

namespace Commands {
    Leave::Leave(std::shared_ptr<BumbleCeepp> Bot) {
        this->Bot = Bot;

        dpp::slashcommand Command = dpp::slashcommand("leave", "음챗을 떠납니다", Bot->BotCluster->me.id);
        dpp::slashcommand Alias = dpp::slashcommand("l", "음챗을 떠납니다", Bot->BotCluster->me.id);

        CommandObjectVector.push_back(Command);
        CommandObjectVector.push_back(Alias);
    }

    void Leave::operator()(std::list<FQueueElement>& MusicQueue, const dpp::slashcommand_t& Event) {
        std::cout << "disconnecting..";

        dpp::voiceconn* v = Event.from->get_voice(Event.command.guild_id);

        if (!v || !v->voiceclient || !v->voiceclient->is_ready()) {
            return;
        }
        
        v->voiceclient->stop_audio();
        Event.from->disconnect_voice(Event.command.guild_id);

        dpp::message msg(Event.command.channel_id, "음성 채팅방을 떠납니다!");

        Event.reply(msg);
    }
}