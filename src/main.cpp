#include <iostream>
#include <BumbleBee.hpp>
#include <AsyncDownloadManager.hpp>
#include <thread>

int main() {
    bumbleBee::BumbleBee bot;

    bumbleBee::AsyncDownloadManager& manager = bumbleBee::AsyncDownloadManager::getInstance(5, bot.cluster, bot.queue);
    manager.enqueue(std::make_pair("https://music.youtube.com/watch?v=4NnqIu_v1QA&si=buZP2UwzQtJLENmb", nullptr));

    std::thread th([](){sleep(60);});
    th.join();
    manager.stop();

    std::cout << "\n\n\n\n\nend\n\n\n\n\n\n\n";
    //bot.start();
}