#include <Audio/MusicPlayManager.hpp>
#include <ogg/ogg.h>
#include <oggz/oggz.h>
#include <algorithm>
#include <Settings/SettingsManager.hpp>

namespace bumbleBee {

void MusicPlayManager::on_voice_ready(const dpp::voice_ready_t& event) {
    play(event.voice_client);
}

void MusicPlayManager::on_voice_track_marker(const dpp::voice_track_marker_t& event) {
    dpp::snowflake gid = dpp::find_channel(event.voice_client->channel_id)->guild_id;
    queueMap[gid]->next_music();
    play(event.voice_client);
}

void MusicPlayManager::on_voice_client_disconnect(const dpp::voice_client_disconnect_t& event) { // 안 불리는 듯?
    dpp::snowflake gid = dpp::find_channel(event.voice_client->channel_id)->guild_id;
    event.voice_client->stop_audio();
    queueMap[gid]->clear();
}

void MusicPlayManager::play(dpp::discord_voice_client* client) {
    std::thread t([&](dpp::discord_voice_client* client){
        dpp::snowflake gid = dpp::find_channel(client->channel_id)->guild_id;
        std::unique_lock<std::mutex> queueEmptyLock(*queueEmptyMutex[gid]);

        queuedCondition.wait(queueEmptyLock, [&]{
            return queueMap[gid]->size() != 0 && queueMap[gid]->currentPlayingPosition != queueMap[gid]->end();
        });

        auto next = queueMap[gid]->currentPlayingPosition;
        send_audio_to_voice(*next, client);
    }, client);
    t.detach();
}

void MusicPlayManager::queue_music(const dpp::snowflake guildId, const std::shared_ptr<MusicQueueElement> music) {
    queueMap[guildId]->enqueue(music);
}

void MusicPlayManager::clear(const dpp::snowflake guildId) {
    queueMap[guildId]->clear();
}
std::shared_ptr<MusicQueueElement> MusicPlayManager::remove(const dpp::snowflake guildId, dpp::discord_voice_client* client, int index) {
    auto queue = queueMap[guildId];
    auto foundIterator = queue->findByIndex(index);

    if (queue->currentPlayingPosition == foundIterator) {
        auto removed = queue->erase(queue->findByIndex(0));
        if (client == nullptr)
            return removed;
        client->pause_audio(true);
        client->stop_audio();
        client->pause_audio(false);
        client->insert_marker("end");
        return removed;
    }

    return queue->erase(queue->findByIndex(index));
}
int MusicPlayManager::size(const dpp::snowflake guildId) {
    return queueMap[guildId]->size();
}
void MusicPlayManager::setRepeat(const dpp::snowflake guildId, const bool value) {
    queueMap[guildId]->repeat = value;
}

bool MusicPlayManager::getRepeat(const dpp::snowflake guildId) {
    return queueMap[guildId]->repeat;
}

std::pair<std::shared_ptr<std::list<std::shared_ptr<MusicQueueElement>>>, std::list<std::shared_ptr<MusicQueueElement>>::iterator>
    MusicPlayManager::getQueue(const dpp::snowflake guildId){
    auto returnValue = queueMap[guildId]->getQueueCopy();
    return returnValue;
}

MusicQueueElement MusicPlayManager::getNowPlaying(const dpp::snowflake guildId) {
    std::shared_ptr<MusicQueueElement> nowplaying = queueMap[guildId]->nowplaying();
    if (nowplaying == nullptr)
        return MusicQueueElement("", "", dpp::user(), dpp::embed());
    MusicQueueElement returnValue(*nowplaying);
    return returnValue;
}

void MusicPlayManager::send_audio_to_voice(std::shared_ptr<bumbleBee::MusicQueueElement> music, dpp::discord_voice_client* client) {
    std::thread t([](std::shared_ptr<bumbleBee::MusicQueueElement> music, dpp::discord_voice_client* client) {
        std::string command = "./streamOpus.sh ";
        command += SettingsManager::getYTDLP_CMD() + " ";
        command += SettingsManager::getFFMPEG_CMD() + " ";
        command += "https://youtu.be/";
        command += music->id;

        OGGZ* og = oggz_open_stdio(popen(command.c_str(), "r"), OGGZ_READ);

        // client->stop_audio(); //이거 필요함??

        oggz_set_read_callback(
            og, -1,
            [](OGGZ *oggz, oggz_packet *packet, long serialno, void *user_data) {
                auto voiceConn = (dpp::discord_voice_client *)user_data;

                voiceConn->send_audio_opus(packet->op.packet, packet->op.bytes);

                return 0;
            },
            (void *)client
        );

        while (client && !client->terminating && music != nullptr) {
            static const constexpr long CHUNK_READ = BUFSIZ * 2;

            const long read_bytes = oggz_read(og, CHUNK_READ);

            /* break on eof */
            if (!read_bytes) {
                break;
            }
        }

        if (music != nullptr)
            client->creator->log(dpp::ll_info, "Sending " + music->embed.title + " - " + music->id + " complete!");
        else {
            client->stop_audio();
            client->pause_audio(true);
        }

        oggz_close(og);

        client->insert_marker();
    }, music, client);
    t.detach();
}
}

