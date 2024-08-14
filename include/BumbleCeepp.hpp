#pragma once
#include <string>
#include <Bot.hpp>
#include <dpp/dpp.h>
#include <memory>
#include <unordered_map>
#include <FQueueElement.hpp>

class BumbleCeepp : public IBot {
public:
    BumbleCeepp(std::string token, std::string DBURL, std::string DBID, std::string DBPassword, int clusterCount = 0);
    ~BumbleCeepp();

    void enqueueMusic(FQueueElement item, dpp::discord_voice_client* vc);
    std::shared_ptr<dpp::embed> findEmbed(std::string musicID);
    bool insertDB(
        std::string webpage_url,
        std::string title,
        std::string uploader,
        std::string id,
        std::string thumbnail,
        time_t duration);
    std::shared_ptr<dpp::embed> makeEmbed(
        std::string webpage_url,
        std::string title,
        std::string uploader,
        std::string id,
        std::string thumbnail,
        time_t duration);

    bool repeat;
    std::string nowPlayingMusic;

private:
    //<guild_id, queueMutex> 쌍임.
    std::unordered_map<dpp::snowflake, std::mutex> enqueuingMutexMap;
    sql::Connection* conn;
};