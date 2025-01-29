#pragma once
#ifndef _MUSICQUEUE_HPP_
#define _MUSICQUEUE_HPP_
#include <memory>
#include <functional>
#include <condition_variable>
#include <list>

namespace bumbleBee {

class MusicQueueElement {
public:
    MusicQueueElement(std::string id, std::string url) : id(id), url(url) {}
    std::string id;
    std::string url;
};

class MusicQueue {
public:
    MusicQueue() {
        currentPlayingPosition = queue.begin();
        repeat = false;
    }
    void enqueue(std::shared_ptr<MusicQueueElement> Element);
    std::shared_ptr<MusicQueueElement> dequeue();
    std::weak_ptr<MusicQueueElement> nowplaying();
    std::weak_ptr<MusicQueueElement> next_music();
    std::weak_ptr<MusicQueueElement> jump_to_index(int idx);

    bool repeat;
    std::function<void()> on_queue_added;
private:
    std::condition_variable queueItemAdded;
    std::mutex queueMutex;
    std::list<std::shared_ptr<MusicQueueElement>> queue;
    std::list<std::shared_ptr<MusicQueueElement>>::iterator currentPlayingPosition;
};
}

#endif