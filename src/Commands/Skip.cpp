#include <Commands/BumbleBeeCommand.hpp>

namespace bumbleBee::commands {
    void Skip::execute(const dpp::slashcommand_t &event) {
        dpp::voiceconn* v = event.from->get_voice(event.command.guild_id);

        if (!v || !v->voiceclient || !v->voiceclient->is_ready()) {
            return;
        }
        // v->voiceclient->pause_audio(true);
        // v->voiceclient->stop_audio();
        // v->voiceclient->pause_audio(false);
        // v->voiceclient->insert_marker("end");
        v->voiceclient->skip_to_next_marker();
        v->voiceclient->insert_marker();

        event.edit_original_response(dpp::message("스킵했습니다!"));
    }

    void Skip::init() {
    }
}