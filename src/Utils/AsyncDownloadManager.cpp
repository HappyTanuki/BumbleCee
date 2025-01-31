#include <Utils/AsyncDownloadManager.hpp>
#include <sstream>
#include "Utils/ConsoleUtils.hpp"
#include "Settings/SettingsManager.hpp"
#include <ogg/ogg.h>
#include <oggz/oggz.h>
#include <opus/opusfile.h>
#include <memory>

namespace bumbleBee {
void AsyncDownloadManager::enqueueAsyncDL(std::pair<std::string, dpp::message> query) {
    std::lock_guard<std::mutex> lock(dlQueueMutex);
    downloadQueue.push(query);
    dlQueueCondition.notify_one();
}

void AsyncDownloadManager::downloadWorker() {
    std::ostringstream tid;
    tid << std::this_thread::get_id();
    while (true) {
        //mutex lock
        std::unique_lock<std::mutex> dlQueueLock(dlQueueMutex);
        dlQueueCondition.wait(dlQueueLock, [&]{ return !downloadQueue.empty() || terminate; });
        auto cluster = weak_cluster.lock();
        if (weak_cluster.expired()) {
            cluster->log(dpp::ll_error, "Missing cluster, terminating thread " + tid.str());
            break;
        }
        if (terminate) {
            cluster->log(dpp::ll_info, "Terminating thread " + tid.str());
            break;
        }
        std::string query = downloadQueue.front().first;
        dpp::message oRes = downloadQueue.front().second;
        downloadQueue.pop();
        dlQueueLock.unlock();

        cluster->log(dpp::ll_info, "AsyncDownloadManager: " + query + " accepted.");

        std::queue<std::string> ids =
            ConsoleUtils::getResultFromCommand(settingsManager::getYTDLP_CMD() +
            " --default-search ytsearch --flat-playlist --skip-download --quiet --ignore-errors --print id " + query);

        if (ids.size() >= 2) {
            cluster->log(dpp::ll_info, query + " is playlist");
            while (!ids.empty()) {
                if (ids.front() == "") {
                    ids.pop();
                    continue;
                }
                cluster->log(dpp::ll_info, "Enqueuing playlist element " + ids.front());
                enqueue(std::make_pair("https://youtu.be/" + ids.front(), oRes));
                ids.pop();
            }
            continue;
        }

        std::queue<std::string> urls =
            ConsoleUtils::getResultFromCommand(settingsManager::getYTDLP_CMD() +
            " -f ba* --print urls https://youtu.be/" + ids.front());

        cluster->log(dpp::ll_debug, "url: " + urls.front());

        FILE* stream;

        // musicQueue->enqueue(std::make_shared<MusicQueueElement>(oRes, ids.front(), urls.front(), stream));

        std::string downloadID = ids.front();

        std::thread th([&, downloadID](){
            if (terminate)
                return;
            std::ostringstream tid;
            tid << std::this_thread::get_id();

            cluster->log(dpp::ll_info, "Thread id: " + tid.str() + ": " + downloadID + " accepted.");

            std::string command = std::string("./streamOpus.sh " + settingsManager::getYTDLP_CMD() + " " + downloadID + " " + settingsManager::getFFMPEG_CMD());
            stream = popen(command.c_str(), "r");

            cluster->log(dpp::ll_info, "Thread id: " + tid.str() + " Opened stream: " + downloadID);
        });
        th.detach();
    }
}
}