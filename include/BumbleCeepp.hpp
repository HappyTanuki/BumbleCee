#ifndef _BUMBLECEEPP_HPP_
#define _BUMBLECEEPP_HPP_
#include <string>
#include <list>
#include <mutex>
#include <Bot.hpp>
#include <dpp/dpp.h>

class BumbleCeepp : public IBot {
public:
    static BumbleCeepp* GetInstance(std::string Token) {
        static BumbleCeepp Instance(Token);
        return &Instance;
    }
    void enqueue(struct FQueueElement Element);

    void QueuePlay();

    uint32_t VoiceJoinedShardId;
    bool Repeat;
protected:
private:
    BumbleCeepp(std::string Token);

    void OnCommand(const dpp::slashcommand_t& Event);

    std::list<struct FQueueElement> MusicQueue;
    std::mutex QueueMutex;
    std::mutex QueuePlayMutex;
    bool QueuePlaying;
};

#endif