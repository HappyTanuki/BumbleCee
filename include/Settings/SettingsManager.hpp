#pragma once
#ifndef _SETTINGSMANAGER_HPP_
#define _SETTINGSMANAGER_HPP_
#include <string>
#include <dpp/nlohmann/json.hpp>
#include <fstream>

namespace bumbleBee {
class settingsManager {
public:
    /// @brief 봇 토큰
    static std::string TOKEN;
    /// @brief yt-dlp 실행 명령어
    static std::string YTDLP_CMD;
    /// @brief ffmpeg 실행 명령어
    static std::string FFMPEG_CMD;
    /// @brief db접속 url
    static std::string DBURL;
    /// @brief db접속 유저
    static std::string DBUSER;
    /// @brief db접속 비밀번호
    static std::string DBPASSWORD;


    static void load();
    
    static void saveToFile();
};
}
#endif