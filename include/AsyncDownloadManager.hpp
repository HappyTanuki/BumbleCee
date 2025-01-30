#pragma once
#ifndef _ASYNCDOWNLOADMANAGER_HPP_
#define _ASYNCDOWNLOADMANAGER_HPP_
#include <queue>
#include <string>
#include <thread>
#include <condition_variable>
#include <dpp/dpp.h>
#include <MusicQueue.hpp>

#define WORKER_COUNT 5

namespace bumbleBee {
/// @brief 싱글톤 멀티스레딩 다운로드 매니저
class AsyncDownloadManager {
public:
    static AsyncDownloadManager& getInstance(int worker_count, std::weak_ptr<dpp::cluster> weak_cluster, std::shared_ptr<bumbleBee::MusicQueue> musicQueue) {
        static AsyncDownloadManager dl(worker_count);
        dl.weak_cluster = weak_cluster;
        dl.musicQueue = musicQueue;
        return dl;
    }
    void enqueue(std::pair<std::string, dpp::message> query) {
        std::thread th(&bumbleBee::AsyncDownloadManager::enqueueAsyncDL, this, query);
        th.detach();
    }
    void stop() {
        auto cluster = weak_cluster.lock();
        cluster->log(dpp::ll_info, "AsyncDownloadManager stop/destructor called.");
        terminate = true;
        dlQueueCondition.notify_all();

        for (auto& t : worker_thread) {
            t.join();
        }
    }

    ~AsyncDownloadManager(){
        stop();
    }

private:
    AsyncDownloadManager(int worker_count){
        worker_thread.reserve(worker_count);
        terminate = false;
        for (int i=0; i<worker_count; i++) {
            worker_thread.emplace_back([this](){this->downloadWorker();});
        }
    }
    
    void enqueueAsyncDL(std::pair<std::string, dpp::message> query);
    void downloadWorker();

    std::queue<std::pair<std::string, dpp::message>> downloadQueue;
    std::condition_variable dlQueueCondition;
    std::mutex dlQueueMutex;
    std::weak_ptr<dpp::cluster> weak_cluster;
    std::shared_ptr<bumbleBee::MusicQueue> musicQueue;
    std::vector<std::thread> worker_thread;
    bool terminate;
};
}

#endif