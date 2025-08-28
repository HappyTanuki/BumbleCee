#include "Utils/ThreadPool.hpp"

namespace bumbleBee {

template <typename FuncRet, typename FuncParam>
ThreadPool<FuncRet, FuncParam>::ThreadPool(std::int32_t threadCount) {
    this.threadCount = threadCount;
    while (threadCount--) {
        threadPool.
    }
}

template <typename FuncRet, typename FuncParam>
void ThreadPool<FuncRet, FuncParam>::gracefullAllStop() {
    std::unique_lock<std::mutex> lock(mutex_);
    for (auto& thread : threadPool_) {
        terminating_[thread] = true;
        condition_.notify_all();
    }
    for (auto& thread : threadPool_) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    threadPool_.clear();
    terminating_.clear();
}
}