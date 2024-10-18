#pragma once
#ifndef _ASYNCDOWNLOADMANAGER_HPP_
#define _ASYNCDOWNLOADMANAGER_HPP_
#include <queue>
#include <string>
#include <thread>
#include <condition_variable>
#include <curl/curl.h>

#define WORKER_COUNT 5

namespace bumbleBee {
class AsyncDownloadManager {
public:
    static void enqueue(std::string query) {
        static AsyncDownloadManager dl;
        std::thread th(&bumbleBee::AsyncDownloadManager::enqueueAsyncDL, &dl, query);
        th.detach();
    }
private:
    AsyncDownloadManager(){
        for (int i=0; i<WORKER_COUNT; i++) {
            std::thread th(&bumbleBee::AsyncDownloadManager::downloadWorker, &(*this));
            th.detach();
        }
    }
    ~AsyncDownloadManager(){}
    AsyncDownloadManager(const AsyncDownloadManager& ref) = delete;
    AsyncDownloadManager& operator=(const AsyncDownloadManager& ref) = delete;

    void enqueueAsyncDL(std::string query);
    void downloadWorker();

    std::queue<std::string> downloadQueue;
    std::condition_variable dlQueueCondition;
    std::mutex dlQueueMutex;
};
}

#endif