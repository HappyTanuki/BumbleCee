#include <Commands/Repeat.hpp>
#include <dpp/dpp.h>
#include <string>

namespace Commands {
    Repeat::Repeat(std::shared_ptr<BumbleCeepp> Bot) {
        this->Bot = Bot;

        dpp::slashcommand Command = dpp::slashcommand("repeat", "반복 켜기/끄기", Bot->BotCluster->me.id);
        dpp::slashcommand Alias = dpp::slashcommand("r", "반복 켜기/끄기", Bot->BotCluster->me.id);

        CommandObjectVector.push_back(Command);
        CommandObjectVector.push_back(Alias);
    }

    void Repeat::operator()(std::list<FQueueElement>& MusicQueue, const dpp::slashcommand_t& Event) {
        if (Bot->Repeat) {
            Event.reply("반복을 껐습니다.");
            Bot->Repeat = false;
        }
        else {
            Event.reply("반복을 켰습니다.");
            Bot->Repeat = true;
        }

        return;
    }
}