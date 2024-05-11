#include <Commands/Delete.hpp>
#include <iostream>

commands::Delete::Delete(dpp::snowflake botID, BumbleCeepp* Bot)
 : ICommand(botID, Bot)
{
    dpp::slashcommand Command = dpp::slashcommand("d", "큐의 해당하는 번호의 노래를 지웁니다", botID);

    Command.add_option(
        dpp::command_option(dpp::co_string, "pos", "큐 번호", botID)
    );

    commandObjectVector.push_back(Command);
}

void commands::Delete::operator()(const dpp::slashcommand_t& event)
{
    if (std::holds_alternative<std::monostate>(event.get_parameter("pos"))) {
        event.reply("삭제할 노래의 인덱스가 정확하지 않습니다.");
        return;
    }
    std::string Pos = std::get<std::string>(event.get_parameter("pos"));
    event.thinking();

    auto index = atoi(Pos.c_str());

    auto vc = event.from->connecting_voice_channels.find(event.command.guild_id)->second->voiceclient;
    int remainingSongsCount = vc->get_tracks_remaining() - 1;
    std::vector<std::string> queuedSongs = vc->get_marker_metadata();

    vc->log(dpp::loglevel::ll_trace, "Queue size : " + remainingSongsCount);

    if (index < 0 || remainingSongsCount+1 < index || (!vc->is_playing() && index == 0)) {
        std::cout << "invalid index : " << index << ", " + Pos + "\n";

        event.edit_original_response(dpp::message(event.command.channel_id, "이상한 인덱스 위치. Pos : " + Pos));
        return;
    }

    dpp::embed embed = Bot->findEmbed(queuedSongs[index - 1])
        .set_timestamp(time(0));

    dpp::message msg(event.command.channel_id, "다음 항목을 큐에서 삭제했습니다!:");

    if (index == 0) {
        dpp::voiceconn* v = event.from->get_voice(event.command.guild_id);

        if (!v || !v->voiceclient || !v->voiceclient->is_ready()) {
            return;
        }

        v->voiceclient->skip_to_next_marker();
    }
    
    msg.add_embed(embed);

    event.edit_original_response(msg);
}