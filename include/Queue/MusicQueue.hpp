#pragma once
#include <memory>
#include <functional>
#include <condition_variable>
#include <list>
#include <dpp/dpp.h>
#include <Queue/MusicQueueElement.hpp>

namespace bumbleBee {

class MusicQueue {
public:
    MusicQueue() {
        queue = std::list<std::shared_ptr<MusicQueueElement>>();
        currentPlayingPosition = queue.begin();
        repeat = true;
    }
    void
        enqueue(std::shared_ptr<MusicQueueElement> Element);
    std::shared_ptr<MusicQueueElement>
        dequeue();
    std::list<std::shared_ptr<MusicQueueElement>>::iterator
        findById(std::string id);
    std::list<std::shared_ptr<MusicQueueElement>>::iterator
        findByIndex(int index);
    std::shared_ptr<MusicQueueElement>
        nowplaying();
    std::list<std::shared_ptr<MusicQueueElement>>::iterator
        next_music();
    std::shared_ptr<MusicQueueElement>
        jump_to_index(int idx);
    void
        clear();
    std::shared_ptr<MusicQueueElement>
        erase(std::list<std::shared_ptr<MusicQueueElement>>::iterator it);
    std::pair<std::shared_ptr<std::list<std::shared_ptr<MusicQueueElement>>>, std::list<std::shared_ptr<MusicQueueElement>>::iterator>
        getQueueCopy();
    int
        size();
    std::list<std::shared_ptr<MusicQueueElement>>::iterator
        end();

    bool repeat;

    std::list<std::shared_ptr<MusicQueueElement>>::iterator currentPlayingPosition;

private:
    std::list<std::shared_ptr<MusicQueueElement>> queue;
    std::mutex queueMutex;
};
}