#include <Commands/BumbleBeeCommand.hpp>

namespace bumbleBee::commands {
    void Skip::execute(const dpp::slashcommand_t &event) {
        dpp::voiceconn* v = event.from->get_voice(event.command.guild_id);

        if (!v || !v->voiceclient || !v->voiceclient->is_ready()) {
            event.edit_original_response(dpp::message("스킵하려면 음악을 재생중이어야 합니다!"));
            return;
        }
        v->voiceclient->stop_audio();
        v->voiceclient->insert_marker();

        event.edit_original_response(dpp::message("스킵했습니다!"));
    }

    void Skip::init() {
    }
}