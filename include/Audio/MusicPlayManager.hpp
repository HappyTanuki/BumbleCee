#pragma once
#ifndef _MUSICPLAYMANAGER_HPP_
#define _MUSICPLAYMANAGER_HPP_
#include <dpp/dpp.h>
#include <Queue/MusicQueue.hpp>
#include <condition_variable>

namespace bumbleBee {
class MusicPlayManager {
public:
    MusicPlayManager(std::shared_ptr<dpp::cluster> cluster) {
        this->cluster = cluster;
        this->queue = std::make_unique<MusicQueue>();

        cluster->on_voice_ready([this](const dpp::voice_ready_t &event){on_voice_ready(event);});
        cluster->on_voice_track_marker([this](const dpp::voice_track_marker_t &event){on_voice_track_marker(event);});
        cluster->on_voice_client_disconnect([this](const dpp::voice_client_disconnect_t& event){on_voice_client_disconnect(event);});
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

    void queue_music(const std::shared_ptr<MusicQueueElement> music);

    std::condition_variable queuedCondition;
private:
    std::shared_ptr<dpp::cluster> cluster;
    /// @brief 음악 큐
    std::unique_ptr<MusicQueue> queue;

    std::mutex queueEmptyMutex;

    void send_audio_to_voice(const MusicQueueElement& music, const dpp::voice_ready_t &event);
    void send_audio_to_voice(const MusicQueueElement& music, const dpp::voice_track_marker_t& event);
};
}
#endif