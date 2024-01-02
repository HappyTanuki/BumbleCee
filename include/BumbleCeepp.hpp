#pragma once
#include <string>
#include <Bot.hpp>
#include <dpp/dpp.h>
#include <MusicQueue.hpp>
#include <memory>
#include <unordered_map>

class BumbleCeepp : public IBot {
public:
    BumbleCeepp(std::string token, int totalShard);
private:
    //<guild_id, queue> 쌍임.
    std::unordered_map<dpp::snowflake, std::shared_ptr<MusicQueue>> queueMap;
};