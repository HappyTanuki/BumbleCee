#include <Commands/Queue.hpp>
#include <iostream>
#include <sstream>
#include <cmath>

commands::Queue::Queue(dpp::snowflake botID, BumbleCeepp* Bot)
 : ICommand(botID, Bot)
{
    dpp::slashcommand command = dpp::slashcommand("q", "노래 예약 큐 확인", botID);

    commandObjectVector.push_back(command);
}

void commands::Queue::operator()(const dpp::slashcommand_t& event) {
    dpp::message msg;
    msg.set_channel_id(event.command.channel_id);

    auto voiceconn = event.from->get_voice(event.command.guild_id);

    if (!voiceconn || !voiceconn->voiceclient) {
        event.reply("음성 채팅방에 참가하지 않은 상태입니다.");
        return;
    }
    auto vc = voiceconn->voiceclient;

    int remainingSongsCount = vc->get_tracks_remaining() - 1;
    if (remainingSongsCount <= 0 && !vc->is_playing()) {
        //재생 중인 노래가 없고 큐에 노래가 없는 상황
        dpp::embed embed = dpp::embed()
            .set_color(dpp::colors::sti_blue)
            .set_title("큐가 비었습니다!")
            .set_timestamp(time(0));

        if (Bot->repeat)
            embed.add_field(":repeat:","");

        msg.add_embed(embed);
    }
    else {
        msg.set_content("지금 재생 중:");
        dpp::embed curMusicEmbed = *Bot->findEmbed(Bot->nowPlayingMusic);
        msg.add_embed(curMusicEmbed);
    }

    event.reply(msg, [&](const dpp::confirmation_callback_t& _event) {
        auto shard_id = dpp::find_guild(event.command.guild_id)->shard_id;
        dpp::cluster* cluster = const_cast<dpp::cluster *>(_event.bot);
        auto shard = cluster->get_shard(shard_id);
        auto iter = shard->connecting_voice_channels.find(event.command.guild_id);
        std::vector<std::string> queuedSongs = iter->second->voiceclient->get_marker_metadata();

        int j;
        for (int i = 0; i < (queuedSongs.size()+4) / 5; i++)
        {
            dpp::embed followEmbed = dpp::embed();
            for (j = i * 5; j < i * 5 + 5 && j < queuedSongs.size(); j++)
            {
                dpp::embed originalEmbed = *Bot->findEmbed(queuedSongs[j]);

                followEmbed.add_field(
                    std::to_string(j + 1),
                    "",
                    true
                )
                .add_field(
                    originalEmbed.title,
                    originalEmbed.description,
                    true
                )
                .add_field(
                    "",
                    ""
                );
            }

            if (j >= queuedSongs.size())
            {
                followEmbed.set_timestamp(time(0));
                if (Bot->repeat)
                    followEmbed.add_field(":repeat:","");
            }

            dpp::message followMsg;
            followMsg.channel_id = event.command.channel_id;

            followMsg.add_embed(followEmbed);
            event.from->creator->message_create(followMsg);
        }
    });
}