#pragma once
#include <memory>
#include <functional>
#include <thread>
#include <condition_variable>
#include <queue>

namespace bumbleBee {
template <typename FuncRet, typename FuncParam>
class ThreadPool {
public:
    ThreadPool() = delete;
    ThreadPool(std::int32_t threadCount);

    std::thread::id execute(std::function<FuncRet(FuncParam)> function);
    void gracefullStop(std::thread::id thread);
    void gracefullAllStop();
private:
    std::mutex mutex_;
    std::condition_variable condition_;
    std::int32_t threadCount_;
    std::vector<std::thread> threadPool_;
    std::unordered_map<std::thread, std::bool> terminating_;
};
}