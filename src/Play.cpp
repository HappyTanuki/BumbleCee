#include <Commands/Play.hpp>
#include <iostream>
#include <dpp/dpp.h>
#include <oggz/oggz.h>
#include <ogg/ogg.h>
#include <opus/opusfile.h>

Play::Play(dpp::snowflake Id) {
    dpp::slashcommand Command = dpp::slashcommand("play", "노래 재생", Id);

    Command.add_option(
        dpp::command_option(dpp::co_string, "query", "링크 또는 검색어", Id)
    );

    dpp::slashcommand Alias = dpp::slashcommand("p", "노래 재생", Id);

    Alias.add_option(
        dpp::command_option(dpp::co_string, "query", "링크 또는 검색어", Id)
    );

    CommandObjectVector.push_back(Command);
    CommandObjectVector.push_back(Alias);
}

void Play::operator()(std::list<std::string>& MusicQueue, const dpp::slashcommand_t& Event) {
    if (std::holds_alternative<std::monostate>(Event.get_parameter("query"))) {
        /* Get the voice channel the bot is in, in this current guild. */
        dpp::voiceconn* v = Event.from->get_voice(Event.command.guild_id);

        /* If the voice channel was invalid, or there is an issue with it, then tell the user. */
        if (!v || !v->voiceclient || !v->voiceclient->is_ready()) {
            Event.reply("There was an issue with getting the voice channel. Make sure I'm in a voice channel!");
            return;
        }

        ogg_sync_state oy; 
        ogg_stream_state os;
        ogg_page og;
        ogg_packet op;
        OpusHead header;
        char *buffer;

        FILE *fd;

        fd = fopen("../audioplayback.ogg", "rb");

        fseek(fd, 0L, SEEK_END);
        size_t sz = ftell(fd);
        rewind(fd);

        ogg_sync_init(&oy);

        buffer = ogg_sync_buffer(&oy, sz);
        fread(buffer, 1, sz, fd);

        ogg_sync_wrote(&oy, sz);

        if (ogg_sync_pageout(&oy, &og) != 1) {
            fprintf(stderr,"Does not appear to be ogg stream.\n");
            exit(1);
        }

        ogg_stream_init(&os, ogg_page_serialno(&og));

        if (ogg_stream_pagein(&os,&og) < 0) {
            fprintf(stderr,"Error reading initial page of ogg stream.\n");
            exit(1);
        }

        if (ogg_stream_packetout(&os,&op) != 1) {
            fprintf(stderr,"Error reading header packet of ogg stream.\n");
            exit(1);
        }

        /* We must ensure that the ogg stream actually contains opus data */
        if (!(op.bytes > 8 && !memcmp("OpusHead", op.packet, 8))) {
            fprintf(stderr,"Not an ogg opus stream.\n");
            exit(1);
        }

        /* Parse the header to get stream info */
        int err = opus_head_parse(&header, op.packet, op.bytes);
        if (err) {
            fprintf(stderr,"Not a ogg opus stream\n");
            exit(1);
        }

        /* Now we ensure the encoding is correct for Discord */
        if (header.channel_count != 2 && header.input_sample_rate != 48000) {
            fprintf(stderr,"Wrong encoding for Discord, must be 48000Hz sample rate with 2 channels.\n");
            exit(1);
        }

        /* Now loop though all the pages and send the packets to the vc */
        while (ogg_sync_pageout(&oy, &og) == 1) {
            ogg_stream_init(&os, ogg_page_serialno(&og));

            if(ogg_stream_pagein(&os,&og)<0) {
                fprintf(stderr,"Error reading page of Ogg bitstream data.\n");
                exit(1);
            }

            while (ogg_stream_packetout(&os,&op) != 0) {

                /* Read remaining headers */
                if (op.bytes > 8 && !memcmp("OpusHead", op.packet, 8)) {
                    int err = opus_head_parse(&header, op.packet, op.bytes);
                    if (err) {
                        fprintf(stderr,"Not a ogg opus stream\n");
                        exit(1);
                    }

                    if (header.channel_count != 2 && header.input_sample_rate != 48000) {
                        fprintf(stderr,"Wrong encoding for Discord, must be 48000Hz sample rate with 2 channels.\n");
                        exit(1);
                    }

                    continue;
                }

                /* Skip the opus tags */
                if (op.bytes > 8 && !memcmp("OpusTags", op.packet, 8))
                    continue; 

                /* Send the audio */
                int samples = opus_packet_get_samples_per_frame(op.packet, 48000);

                v->voiceclient->send_audio_opus(op.packet, op.bytes, samples / 48);
            }
        }

        /* Cleanup */
        ogg_stream_clear(&os);
        ogg_sync_clear(&oy);

        Event.reply("Finished playing the audio file!");
    }
}