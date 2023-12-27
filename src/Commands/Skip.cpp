#include <Commands/Skip.hpp>
#include <dpp/dpp.h>
#include <string>

namespace Commands {
    Skip::Skip(std::shared_ptr<BumbleCeepp> Bot) {
        this->Bot = Bot;

        dpp::slashcommand Command = dpp::slashcommand("skip", "현재곡 스킵", Bot->BotCluster->me.id);
        dpp::slashcommand Alias = dpp::slashcommand("s", "현재곡 스킵", Bot->BotCluster->me.id);

        CommandObjectVector.push_back(Command);
        CommandObjectVector.push_back(Alias);
    }

    void Skip::operator()(std::list<FQueueElement>& MusicQueue, const dpp::slashcommand_t& Event) {
        dpp::voiceconn* v = Event.from->get_voice(Event.command.guild_id);

        if (!v || !v->voiceclient || !v->voiceclient->is_ready()) {
            return;
        }

        v->voiceclient->stop_audio();

        Event.reply("스킵했습니다!");

        return;
    }
}