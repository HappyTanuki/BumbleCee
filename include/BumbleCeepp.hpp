#ifndef _BUMBLECEEPP_HPP_
#define _BUMBLECEEPP_HPP_
#include <string>
#include <list>
#include <mutex>
#include <Bot.hpp>
#include <dpp/dpp.h>

class BumbleCeepp : public IBot {
public:
    BumbleCeepp(std::string Token, int TotalShard);
    void enqueue(struct FQueueElement Element);
    struct FQueueElement QueueDelete(int Index);

    void QueuePlay();

    uint32_t VoiceJoinedShardId;
    bool Repeat;
    std::mutex YTDLMutex;
    std::list<struct FQueueElement> MusicQueue;
protected:
private:
    void OnCommand(const dpp::slashcommand_t& Event);
    std::mutex QueueMutex;
    bool QueuePlaying;
};

#endif