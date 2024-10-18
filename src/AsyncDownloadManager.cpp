#include <AsyncDownloadManager.hpp>

namespace bumbleBee {

void AsyncDownloadManager::enqueueAsyncDL(std::string query) {
    dlQueueMutex.lock();
    downloadQueue.push(query);
    dlQueueMutex.unlock();
    dlQueueCondition.notify_one();
}

void AsyncDownloadManager::downloadWorker() {
    std::unique_lock<std::mutex> dlQueueLock(dlQueueMutex);
    while (true) {
        //mutex lock
        dlQueueLock.lock();
        dlQueueCondition.wait(dlQueueLock, [&]{ return !downloadQueue.empty(); });

        system(("./yt-dlp --no-clean-info-json --write-info-json --default-search ytsearch \
            --flat-playlist --skip-download --quiet --ignore-errors -f ba* " + downloadQueue.front()).c_str());
        downloadQueue.pop();
        dlQueueLock.unlock();
    }
}
}