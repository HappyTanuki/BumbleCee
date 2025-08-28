#pragma once
#include <memory>
#include <dpp/dpp.h>
#include <Queue/MusicQueueElement.hpp>
#include "Utils/ThreadPool.hpp"

namespace bumbleBee {
class ThreadManager : public ThreadPool<dpp::snowflake, int, int> {
public:
    bool addMusic(std::shared_ptr<bumbleBee::MusicQueueElement> music, dpp::discord_voice_client* client);
    void stopSending(dpp::snowflake gid);
private:
    // GID, 쓰레드
    std::unordered_map<dpp::snowflake, std::thread> threadPool;
    // GID, 쓰레드 종료
    std::unordered_map<dpp::snowflake, bool> terminating;
};
}