#pragma once
#include <Bot.hpp>

class BumbleCee : public IBot {
public:
    static BumbleCee* getInstance() {
        static BumbleCee instance;
        return &instance;
    }
protected:
private:
    BumbleCee() {}
    BumbleCee(const BumbleCee& ref) {}
    ~BumbleCee() {}
};