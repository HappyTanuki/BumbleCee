#include "BumbleCeepp.hpp"
#include <string>
#include <ogg/ogg.h>
#include <opus/opusfile.h>
#include <FQueueElement.hpp>

BumbleCeepp::BumbleCeepp(std::string Token) : IBot(Token) {
    VoiceJoinedShardId = 0;
}

void BumbleCeepp::enqueue(struct FQueueElement Element) {
    QueueMutex.lock();
    MusicQueue.push_back(Element);
    QueueMutex.unlock();
}

void BumbleCeepp::QueuePlay(){
    dpp::discord_client* JoinedShared = BotCluster->get_shard(VoiceJoinedShardId);
    if (!JoinedShared) {
        return;
    }

    while (!MusicQueue.empty()) {
        QueueMutex.lock();
        FQueueElement Music = MusicQueue.front();
        MusicQueue.pop_front();
        QueueMutex.unlock();

        dpp::voiceconn* v = JoinedShared->get_voice(Music.guild_id);
        if (!v || !v->voiceclient || !v->voiceclient->is_ready()) {
            return;
        }

        ogg_sync_state oy; 
        ogg_stream_state os;
        ogg_page og;
        ogg_packet op;
        OpusHead header;
        char *buffer;

        FILE *fd;

        fd = fopen((std::string(Music.FileName.c_str()) + ".ogg").c_str(), "rb");
        if (!fd) {
            continue;
        }

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

        std::list<ogg_packet> OpusList;

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

                ogg_packet D;

                memcpy(&D, &op, sizeof(ogg_packet));

                OpusList.push_back(D);
                //v->voiceclient->send_audio_opus(op.packet, op.bytes, samples / 48);
            }
        }

        while (!OpusList.empty()) {
            ogg_packet D = OpusList.front();
            OpusList.pop_front();

            v->voiceclient->send_audio_opus(D.packet, D.bytes);
        }

        /* Cleanup */
        ogg_stream_clear(&os);
        ogg_sync_clear(&oy);
        system(("rm -f " + std::string(Music.FileName.c_str()) + ".ogg").c_str());
    }
}

void BumbleCeepp::OnCommand(const dpp::slashcommand_t& Event) {
    for (auto Command : CommandsArray) {
        for (auto Alias : Command->CommandObjectVector) {
            if (Event.command.get_command_name() == Alias.name) {
                (*Command)(Event);
                (*Command)(MusicQueue, Event);
            }
        }
    }
}