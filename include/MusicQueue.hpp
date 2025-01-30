#pragma once
#ifndef _MUSICQUEUE_HPP_
#define _MUSICQUEUE_HPP_
#include <memory>
#include <functional>
#include <condition_variable>
#include <list>
#include <dpp/dpp.h>

namespace bumbleBee {

class MusicQueueElement {
public:
    MusicQueueElement(
        dpp::message originalResponse,
        std::string id,
        std::string url,
        FILE* stream) :

        originalResponse(originalResponse), id(id), url(url), stream(stream) {}
    dpp::message originalResponse;
    std::string id;
    std::string url;
    FILE* stream;
};

class MusicQueue {
public:
    MusicQueue() {
        currentPlayingPosition = queue.begin();
        repeat = false;
    }
    void enqueue(std::shared_ptr<MusicQueueElement> Element);
    std::shared_ptr<MusicQueueElement> dequeue();
    std::shared_ptr<MusicQueueElement> findById(std::string id);
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