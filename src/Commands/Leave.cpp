#include <Commands/BumbleBeeCommand.hpp>

namespace bumbleBee::commands {
    void Leave::execute(const dpp::slashcommand_t &event) {
        dpp::voiceconn* v = event.from->get_voice(event.command.guild_id);

        if (!v || !v->voiceclient || !v->voiceclient->is_ready()) {
            event.edit_original_response(dpp::message("현재 음성 채팅방에 있는 상태가 아닙니다!"));
            return;
        }
        musicManager->clear(event.command.guild_id);
        
        v->voiceclient->stop_audio();
        v->voiceclient->pause_audio(true);
        event.from->clear_queue();
        event.from->disconnect_voice(event.command.guild_id);

        event.edit_original_response(dpp::message("음성 채팅방을 떠납니다!"));
    }

    void Leave::init() {
    }
}