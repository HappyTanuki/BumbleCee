#include <iostream>
#include <BumbleBee.hpp>
#include <AsyncDownloadManager.hpp>
#include <thread>

int main() {
    nlohmann::json configdocument;
    std::ifstream configfile("config.json");
    configfile >> configdocument;

    bumbleBee::BumbleBee bot(configdocument);

    bumbleBee::AsyncDownloadManager& manager = bumbleBee::AsyncDownloadManager::getInstance(5, bot.cluster, bot.queue);
    manager.enqueue("https://music.youtube.com/playlist?list=PL5NSTAfQ-wQBqZYMTqxADemyUW8mxJq2h&si=vFV4jlm70kxGfKNa");

    std::thread th([](){sleep(11);});
    th.join();

    std::cout << "\n\n\n\n\nend\n\n\n\n\n\n\n";
    //bot.start();
}