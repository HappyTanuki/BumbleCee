#pragma once
#include <dpp/dpp.h>
#include <Queue/MusicQueue.hpp>
#include <condition_variable>

namespace bumbleBee {
class MusicPlayManager {
public:
    MusicPlayManager(std::shared_ptr<dpp::cluster> cluster, std::vector<dpp::snowflake> GIDs) :
        cluster(cluster), GIDs(GIDs) {
        queueMap = std::unordered_map<dpp::snowflake, std::shared_ptr<MusicQueue>>();
        queueEmptyMutex = std::unordered_map<dpp::snowflake, std::shared_ptr<std::mutex>>();

        cluster->on_voice_ready([this](const dpp::voice_ready_t &event){on_voice_ready(event);});
        cluster->on_voice_track_marker([this](const dpp::voice_track_marker_t &event){on_voice_track_marker(event);});
        cluster->on_voice_client_disconnect([this](const dpp::voice_client_disconnect_t& event){on_voice_client_disconnect(event);});

        for (auto GID : GIDs) {
            queueMap[GID] = std::make_shared<MusicQueue>();
            queueEmptyMutex[GID] = std::make_shared<std::mutex>();
        }
    }

    /**
     * @brief voice_ready 이벤트 인지시 콜백되는 함수
     * @param event const dpp::voice_ready_t&
    **/
    void on_voice_ready(const dpp::voice_ready_t& event);
    /**
     * @brief voice_track_marker 이벤트 인지시 콜백되는 함수
     * @param event const dpp::voice_track_marker_t&
    **/
    void on_voice_track_marker(const dpp::voice_track_marker_t& event);
    /**
     * @brief voice_client_disconnect 이벤트 인지시 콜백되는 함수
     * @param event const dpp::voice_client_disconnect_t&
    **/
    void on_voice_client_disconnect(const dpp::voice_client_disconnect_t& event);

    void play(dpp::discord_voice_client* client);

    void queue_music(const dpp::snowflake guildId, const std::shared_ptr<MusicQueueElement> music);

    void clear(const dpp::snowflake guildId);
    std::shared_ptr<MusicQueueElement> remove(const dpp::snowflake guildId, dpp::discord_voice_client* client, int index);
    int size(const dpp::snowflake guildId);

    void setRepeat(const dpp::snowflake guildId, const bool value);
    bool getRepeat(const dpp::snowflake guildId);

    std::pair<std::shared_ptr<std::list<std::shared_ptr<MusicQueueElement>>>, std::list<std::shared_ptr<MusicQueueElement>>::iterator> getQueue(const dpp::snowflake guildId);
    MusicQueueElement getNowPlaying(const dpp::snowflake guildId);

    std::condition_variable queuedCondition;
    
private:
    std::shared_ptr<dpp::cluster> cluster;

    std::vector<dpp::snowflake> GIDs;
    /// @brief 음악 큐
    std::unordered_map<dpp::snowflake, std::shared_ptr<MusicQueue>> queueMap;

    std::unordered_map<dpp::snowflake, std::shared_ptr<std::mutex>> queueEmptyMutex;

    void send_audio_to_voice(std::shared_ptr<bumbleBee::MusicQueueElement> music, dpp::discord_voice_client* client);
};
}