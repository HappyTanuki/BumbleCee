#include <Queue/MusicQueue.hpp>
#include <iostream>

namespace bumbleBee {

void MusicQueue::enqueue(std::shared_ptr<MusicQueueElement> Element) {
    std::lock_guard<std::mutex> lock(queueMutex);
        queue.push_back(Element);
    if (queue.size() == 1) // 이전에 하나도 없었다는 말이므로.
        currentPlayingPosition = queue.begin(); // 첫번째 곡으로 iterator를 옮겨준다.
}

std::shared_ptr<MusicQueueElement> MusicQueue::dequeue() {
    std::lock_guard<std::mutex> lock(queueMutex);
    auto value = queue.front();
    queue.pop_front();
    return value;
}
std::list<std::shared_ptr<MusicQueueElement>>::iterator MusicQueue::findById(std::string id) {
    std::lock_guard<std::mutex> lock(queueMutex);
    int index = 0;
    for (auto iter = queue.begin(); iter != queue.end(); iter++) {
        if ((*iter).get()->id == id)
            return iter;
    }
    return queue.end();
}
std::shared_ptr<MusicQueueElement> MusicQueue::nowplaying() {
    return *currentPlayingPosition;
}
std::list<std::shared_ptr<MusicQueueElement>>::iterator MusicQueue::next_music() {
    std::lock_guard<std::mutex> lock(queueMutex);
    if (currentPlayingPosition == std::prev(queue.end()) || !repeat)
        return queue.end();
    if (currentPlayingPosition == std::prev(queue.end()) || repeat)
        currentPlayingPosition = queue.begin();
    else
        ++currentPlayingPosition;
    return currentPlayingPosition;
}
std::shared_ptr<MusicQueueElement> MusicQueue::jump_to_index(int idx) {
    std::lock_guard<std::mutex> lock(queueMutex);
    int index = 0;
    for (auto iter = queue.begin(); iter != queue.end(); iter++) {
        if (idx == index++) {
            currentPlayingPosition = iter;
            return *iter;
        }
    }
    return std::shared_ptr<MusicQueueElement>();
}
}
