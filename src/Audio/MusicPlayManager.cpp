#include <Audio/MusicPlayManager.hpp>
#include <ogg/ogg.h>
#include <oggz/oggz.h>

namespace bumbleBee {

void MusicPlayManager::on_voice_ready(const dpp::voice_ready_t& event) {
    std::unique_lock<std::mutex> queueEmptyLock(queueEmptyMutex);
    queuedCondition.wait(queueEmptyLock, [&]{
        return queue->queue.size() != 0;
    });

    auto np = queue->nowplaying();
    auto music = *np;
    
    std::string command = "./streamOpus.sh ./yt-dlp ffmpeg https://youtu.be/";
    command += music.id;

    OGGZ* og = oggz_open_stdio(popen(command.c_str(), "r"), OGGZ_READ);

    oggz_set_read_callback(
        og, -1,
        [](OGGZ *oggz, oggz_packet *packet, long serialno, void *user_data) {
            auto voiceConn = (dpp::discord_voice_client *)user_data;

            voiceConn->send_audio_opus(packet->op.packet, packet->op.bytes);

            return 0;
        },
        (void *)event.voice_client
    );

    while (event.voice_client && !event.voice_client->terminating) {
        static const constexpr long CHUNK_READ = BUFSIZ * 2;

        const long read_bytes = oggz_read(og, CHUNK_READ);

        /* break on eof */
        if (!read_bytes) {
            break;
        }
    }

    //event.from->creator->log(dpp::ll_info, std::string("Sending ") + music.id + " complete!");
    oggz_close(og);

    event.voice_client->insert_marker("end");

    queueEmptyLock.unlock();
}

void MusicPlayManager::on_voice_track_marker(const dpp::voice_track_marker_t& event) {
    queue->next_music();
}

void MusicPlayManager::on_voice_client_disconnect(const dpp::voice_client_disconnect_t& event) {
    event.voice_client->stop_audio();
}

void MusicPlayManager::queue_music(const std::shared_ptr<MusicQueueElement> music) {
    queue->enqueue(music);
}

void MusicPlayManager::send_audio_to_voice(const MusicQueueElement& music, const dpp::voice_ready_t &event) {
    
}
}

