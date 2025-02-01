#include <Commands/BumbleBeeCommand.hpp>

namespace bumbleBee::commands {
    void Leave::execute(const dpp::slashcommand_t &event) { // 왜 read loop ended가 뜨는가...
        dpp::voiceconn* v = event.from->get_voice(event.command.guild_id);

        if (!v || !v->voiceclient || !v->voiceclient->is_ready()) {
            return;
        }
        musicManager->clear(event.command.guild_id);
        
        event.from->disconnect_voice(event.command.guild_id);

        event.edit_original_response(dpp::message("음성 채팅방을 떠납니다!"));
    }

    void Leave::init() {
    }
}