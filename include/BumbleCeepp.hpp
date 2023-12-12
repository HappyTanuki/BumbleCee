#pragma once
#include "Bot.hpp"
#include <string>
#include <list>

class BumbleCeepp : public IBot {
public:
    static BumbleCeepp* GetInstance(std::string Token) {
        static BumbleCeepp Instance(Token);
        return &Instance;
    }
    void enqueue(std::string);
    std::string dequeue();
protected:
    std::list<std::string> MusicQueue;
private:
    BumbleCeepp(std::string Token);

    void OnCommand(const dpp::slashcommand_t& Event);
};