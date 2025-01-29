#pragma once
#ifndef _ASYNCDOWNLOADMANAGER_HPP_
#define _ASYNCDOWNLOADMANAGER_HPP_
#include <queue>
#include <string>
#include <thread>
#include <condition_variable>
#include <curl/curl.h>
#include <dpp/dpp.h>
#include <MusicQueue.hpp>

#define WORKER_COUNT 5

namespace bumbleBee {
class AsyncDownloadManager {
public:
    static AsyncDownloadManager& getInstance(int worker_count, std::weak_ptr<dpp::cluster> bot, std::shared_ptr<bumbleBee::MusicQueue> musicQueue) {
        static AsyncDownloadManager dl(worker_count);
        dl.bot = bot;
        dl.musicQueue = musicQueue;
        return dl;
    }
    void enqueue(std::string query) {
        std::thread th(&bumbleBee::AsyncDownloadManager::enqueueAsyncDL, this, query);
        th.detach();
    }

    ~AsyncDownloadManager(){
        auto cluster = bot.lock();
        assert(cluster);
        cluster->log(dpp::ll_info, "AsyncDownloadManager Destructor called.");
        terminate = true;
        dlQueueCondition.notify_all();

        for (auto& t : worker_thread) {
            t.join();
        }
    }

private:
    AsyncDownloadManager(int worker_count){
        worker_thread.reserve(worker_count);
        terminate = false;
        for (int i=0; i<worker_count; i++) {
            worker_thread.emplace_back([this](){this->downloadWorker();});
        }
    }
    
    void enqueueAsyncDL(std::string query);
    void downloadWorker();

    std::queue<std::string> downloadQueue;
    std::condition_variable dlQueueCondition;
    std::mutex dlQueueMutex;
    std::weak_ptr<dpp::cluster> bot;
    std::shared_ptr<bumbleBee::MusicQueue> musicQueue;
    std::vector<std::thread> worker_thread;
    bool terminate;
};
}

#endif