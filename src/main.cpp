#include <iostream>
#include <BumbleBee.hpp>

int main() {
    nlohmann::json configdocument;
    std::ifstream configfile("config.json");
    configfile >> configdocument;

    bumbleBee::BumbleBee bot(configdocument);

    bot.start();
}