#include <Commands/BumbleBeeCommand.hpp>
#include <format>

namespace bumbleBee::commands {
    void Delete::execute(const dpp::slashcommand_t &event) {
        if (std::holds_alternative<std::monostate>(event.get_parameter("pos"))) // 여기 들어올 일 있나?
        {
            event.edit_original_response(dpp::message("위치를 제공하여 주십시오"));
            event.reply("");
            return;
        }
        int pos = std::get<std::int64_t>(event.get_parameter("pos"));

        if (pos < 0 || pos > musicManager->size(event.command.guild_id))
        {

            event.edit_original_response(dpp::message(std::string("이상한 인덱스 위치. Pos :") + std::to_string(pos)));
            return;
        }

        dpp::voiceconn* v = event.from->get_voice(event.command.guild_id);

        std::shared_ptr<MusicQueueElement> removed;
        
        if (!v) // v-> 로 nullptr을 참조하면 안 되므로.
            removed = musicManager->remove(event.command.guild_id, nullptr, pos);
        else
            removed = musicManager->remove(event.command.guild_id, v->voiceclient, pos);

        dpp::message msg("다음 항목을 큐에서 삭제했습니다!:");
        msg.add_embed(removed->embed);

        event.edit_original_response(msg);
    }

    void Delete::init() {
        add_option(dpp::command_option(dpp::co_integer, "pos", "지울 위치(위치는 1부터 시작, 현재 재생곡은 0)", true));
    }
}