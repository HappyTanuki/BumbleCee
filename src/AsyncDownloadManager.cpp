#include <AsyncDownloadManager.hpp>
#include <sstream>

namespace bumbleBee {

void AsyncDownloadManager::enqueueAsyncDL(std::string query) {
    std::lock_guard<std::mutex> lock(dlQueueMutex);
    downloadQueue.push(query);
    dlQueueCondition.notify_one();
}

std::string getResultFromCommand(std::string cmd) {
	std::string result;
	FILE* stream;
	const int maxBuffer = 12; // 버퍼의 크기는 적당하게
	char buffer[maxBuffer];
	cmd.append(" 2>&1"); // 표준에러를 표준출력으로 redirect

    stream = popen(cmd.c_str(), "r"); // 주어진 command를 shell로 실행하고 파이프 연결 (fd 반환)
    	if (stream) {
    		while (fgets(buffer, maxBuffer, stream) != NULL) result.append(buffer); // fgets: fd (stream)를 길이 (maxBuffer)만큼 읽어 버퍼 (buffer)에 저장
    		pclose(stream); // 파이프 닫는 것 잊지 마시고요!
    	}
	return result;
}

void AsyncDownloadManager::downloadWorker() {
    std::ostringstream tid;
    tid << std::this_thread::get_id();
    while (true) {
        //mutex lock
        std::unique_lock<std::mutex> dlQueueLock(dlQueueMutex);
        dlQueueCondition.wait(dlQueueLock, [&]{ return !downloadQueue.empty() || terminate; });
        std::string query = downloadQueue.front();
        downloadQueue.pop();
        dlQueueLock.unlock();
        auto cluster = bot.lock();
        assert(cluster);

        cluster->log(dpp::ll_info, "Enqueuing " + query + " accepted.");

        std::string idstring = getResultFromCommand("./yt-dlp --default-search ytsearch --flat-playlist --skip-download --quiet --ignore-errors --print id " + query);
        std::queue<std::string> ids;
        std::stringstream ss(idstring);

        std::string _id;
        while (std::getline(ss, _id, '\n')) {
            ids.push(_id);
        }

        if (ids.size() >= 2) {
            cluster->log(dpp::ll_info, "Playlist detected.");
            while (!ids.empty()) {
                cluster->log(dpp::ll_info, "Enqueuing " + ids.front());
                enqueue("https://youtu.be/" + ids.front());
                ids.pop();
            }
            break;
        }

        cluster->log(dpp::ll_info, "Thread id: " + tid.str() + ": " + ids.front() + " accepted.");

        system(("./yt-dlp -o \"Temp/%(id)s\" --no-clean-info-json --write-info-json --default-search ytsearch \
            --flat-playlist --skip-download --quiet --ignore-errors -f ba* https://youtu.be/" + ids.front()).c_str());

        cluster->log(dpp::ll_info, "Thread id: " + tid.str() + ": " + ids.front() + " downloaded.");
    }
}
}