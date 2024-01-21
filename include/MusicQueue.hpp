#pragma once
#include <list>
#include <FQueueElement.hpp>

struct FMusicQueueID {
    dpp::snowflake guild_id;
    uint32_t shard_id;
};

class MusicQueue {
public:
    MusicQueue(FMusicQueueID id, std::shared_ptr<dpp::cluster> botCluster);
    void operator+=(FQueueElement operand);
    FQueueElement pop(int index);
    FQueueElement peek(int index);
    bool empty();
    void clear();
    std::list<struct FQueueElement>::iterator begin();
    std::list<struct FQueueElement>::iterator end();
    std::size_t size();
    FMusicQueueID getId();
    void play();

    void markerCallback();

    bool repeat;
private:
    std::list<struct FQueueElement> queue;
    std::mutex mutex;
    std::mutex playMutex;
    FMusicQueueID id;
    std::shared_ptr<dpp::cluster> botCluster;
};