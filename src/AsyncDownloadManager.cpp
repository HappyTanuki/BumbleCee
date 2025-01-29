#include <AsyncDownloadManager.hpp>
#include <sstream>

namespace bumbleBee {

void AsyncDownloadManager::enqueueAsyncDL(std::string query) {
    std::lock_guard<std::mutex> lock(dlQueueMutex);
    downloadQueue.push(query);
    dlQueueCondition.notify_one();
}

std::queue<std::string> getResultFromCommand(std::string cmd) {
	std::string result, token;
    std::queue<std::string> tokens;
	FILE* stream;
	const int maxBuffer = 12; // 버퍼의 크기는 적당하게
	char buffer[maxBuffer];
	cmd.append(" 2>&1"); // 표준에러를 표준출력으로 redirect

    stream = popen(cmd.c_str(), "r"); // 주어진 command를 shell로 실행하고 파이프 연결 (fd 반환)
    	if (stream) {
    		while (fgets(buffer, maxBuffer, stream) != NULL) result.append(buffer); // fgets: fd (stream)를 길이 (maxBuffer)만큼 읽어 버퍼 (buffer)에 저장
    		pclose(stream);
    	}

    std::stringstream ss(result);
    while (std::getline(ss, token, '\n')) {
        tokens.push(token);
    }

	return tokens;
}

void AsyncDownloadManager::downloadWorker() {
    std::ostringstream tid;
    tid << std::this_thread::get_id();
    while (true) {
        //mutex lock
        std::unique_lock<std::mutex> dlQueueLock(dlQueueMutex);
        dlQueueCondition.wait(dlQueueLock, [&]{ return !downloadQueue.empty() || terminate; });
        auto cluster = bot.lock();
        assert(cluster);
        if (terminate) {
            cluster->log(dpp::ll_info, "Terminating Thread" + tid.str());
            break;
        }
        std::string query = downloadQueue.front();
        downloadQueue.pop();
        dlQueueLock.unlock();

        cluster->log(dpp::ll_info, "Enqueuing " + query + " accepted.");

        std::queue<std::string> ids = getResultFromCommand("./yt-dlp --default-search ytsearch --flat-playlist --skip-download --quiet --ignore-errors --print id " + query);

        if (ids.size() >= 2) {
            while (!ids.empty()) {
                if (ids.front() == "") {
                    ids.pop();
                    continue;
                }
                cluster->log(dpp::ll_info, "Enqueuing " + ids.front());
                enqueue("https://youtu.be/" + ids.front());
                ids.pop();
            }
            break;
        }

        std::queue<std::string> urls = getResultFromCommand("./yt-dlp -f ba* --print urls https://youtu.be/" + ids.front());

        cluster->log(dpp::ll_info, "url: " + urls.front());

        musicQueue->enqueue(std::make_shared<MusicQueueElement>(ids.front(), urls.front()));

        std::string downloadID = ids.front();

        std::thread th([&, downloadID](){
            if (terminate)
                return;
            std::ostringstream tid;
            tid << std::this_thread::get_id();

            cluster->log(dpp::ll_info, "Thread id: " + tid.str() + ": " + downloadID + " accepted.");

            system(("./yt-dlp -o \"Temp/%(id)s\" --quiet --ignore-errors -f ba* https://youtu.be/" + downloadID).c_str());
            
            system((std::string() + "yes n 2>/dev/null | ffmpeg -hide_banner -loglevel error -i \""
                + "Temp/" + downloadID + "\" -acodec libopus -vn Music/" + downloadID + ".ogg").c_str());
            system((std::string() + "rm -f Temp/" + downloadID).c_str());
            
            cluster->log(dpp::ll_info, "Thread id: " + tid.str() + ": " + downloadID + " downloaded.");
        });
        th.detach();
    }
}
}