#include <Commands/BumbleBeeCommand.hpp>

namespace bumbleBee::commands {
    void Repeat::execute(const dpp::slashcommand_t &event) {
        if (musicManager->getRepeat(event.command.guild_id)) {
            event.edit_original_response(dpp::message("반복을 껐습니다."));
            musicManager->setRepeat(event.command.guild_id, false);
        }
        else {
            event.edit_original_response(dpp::message("반복을 켰습니다."));
            musicManager->setRepeat(event.command.guild_id, true);
        }
    }

    void Repeat::init() {}
}