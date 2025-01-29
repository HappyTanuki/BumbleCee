#include <MusicQueue.hpp>
#include <iostream>

namespace bumbleBee {

void MusicQueue::enqueue(std::shared_ptr<MusicQueueElement> Element) {
    std::lock_guard<std::mutex> lock(queueMutex);
    queue.push_back(Element);
}

std::shared_ptr<MusicQueueElement> MusicQueue::dequeue() {
    std::lock_guard<std::mutex> lock(queueMutex);
    auto value = std::move(queue.front());
    queue.pop_front();
    return value;
}
std::weak_ptr<MusicQueueElement> MusicQueue::nowplaying() {
    return *currentPlayingPosition;
}
std::weak_ptr<MusicQueueElement> MusicQueue::next_music() {
    std::lock_guard<std::mutex> lock(queueMutex);
    if (currentPlayingPosition == queue.end())
        return *currentPlayingPosition;

    ++currentPlayingPosition;

    if (repeat && currentPlayingPosition == queue.end()) {
        currentPlayingPosition = queue.begin();
    }
    return *currentPlayingPosition;
}
std::weak_ptr<MusicQueueElement> MusicQueue::jump_to_index(int idx) {
    std::lock_guard<std::mutex> lock(queueMutex);
    int index = 0;
    for (auto iter = queue.begin(); iter != queue.end(); iter++) {
        if (idx == index++) {
            currentPlayingPosition = iter;
            return *iter;
        }
    }
    std::shared_ptr<MusicQueueElement> empty;
    return empty;
}
}