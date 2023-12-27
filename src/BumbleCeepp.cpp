#include "BumbleCeepp.hpp"
#include <string>
#include <ogg/ogg.h>
#include <oggz/oggz.h>
#include <opus/opusfile.h>
#include <FQueueElement.hpp>
#include <thread>

BumbleCeepp::BumbleCeepp(std::string Token) : IBot(Token) {
    VoiceJoinedShardId = 0;
}

void BumbleCeepp::enqueue(struct FQueueElement Element) {
    QueueMutex.lock();
    MusicQueue.push_back(Element);
    QueueMutex.unlock();
}

struct FQueueElement BumbleCeepp::QueueDelete(int Index) {
    QueueMutex.lock();
    auto iter = MusicQueue.begin();
    std::advance(iter, Index);
    auto ReturnValue = *iter;
    MusicQueue.erase(iter);
    QueueMutex.unlock();

    return ReturnValue;
}

void BumbleCeepp::QueuePlay(){
    if (QueuePlaying) {
        return;
    }
    QueuePlaying = true;

    std::thread T1([this] (){
        dpp::discord_client* JoinedShared = BotCluster->get_shard(VoiceJoinedShardId);
        if (!JoinedShared) {
            return;
        }

        while (!MusicQueue.empty()) {
            QueueMutex.lock();
            FQueueElement Music = MusicQueue.front();
            QueueMutex.unlock();

            dpp::voiceconn* v = JoinedShared->get_voice(Music.guild_id);
            if (!v || !v->voiceclient || !v->voiceclient->is_ready()) {
                return;
            }

            /* load the audio file with oggz */
            OGGZ *track_og = oggz_open(("Music/" + std::string(Music.FileName.c_str()) + ".ogg").c_str(), OGGZ_READ);

            /* If there was an issue reading the file, tell the user and stop */
            if (!track_og) {
                fprintf(stderr, "Error opening file\n");
                return;
            }

            /* set read callback, this callback will be called on packets with the serialno,
                * -1 means every packet will be handled with this callback.
                */
            oggz_set_read_callback(
                track_og, -1,
                [](OGGZ *oggz, oggz_packet *packet, long serialno, void *user_data) {
                    dpp::voiceconn *voiceconn = (dpp::voiceconn *)user_data;

                    /* send the audio */
                    voiceconn->voiceclient->send_audio_opus(packet->op.packet, packet->op.bytes);

                    /* make sure to always return 0 here, read more on oggz documentation */
                    return 0;
                },
                /* this will be the value of void *user_data */
                (void *)v
            );

            // read loop
            while (v && v->voiceclient && !v->voiceclient->terminating) {
                /* you can tweak this to whatever. Here I use BUFSIZ, defined in
                    * stdio.h as 8192.
                    */
                static const constexpr long CHUNK_READ = BUFSIZ * 2;

                const long read_bytes = oggz_read(track_og, CHUNK_READ);

                /* break on eof */
                if (!read_bytes) {
                    break;
                }
            }

            /* Don't forget to free the memory */
            oggz_close(track_og);

            std::cout << "audio sending complete\n";

            int n = 0;

            while(v->voiceclient->is_playing()) {}

            QueueMutex.lock();
            if (MusicQueue.empty()) {
                QueueMutex.unlock();
                break;
            }
            MusicQueue.pop_front();
            QueueMutex.unlock();

            if (Repeat) {
                QueueMutex.lock();
                MusicQueue.push_back(Music);
                QueueMutex.unlock();
            }
        }
        QueuePlaying = false;

        std::cout << "Queue ended\n";
    });
    T1.detach();
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