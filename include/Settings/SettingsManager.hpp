#pragma once
#include <dpp/dpp.h>

#define _DECLARE_DEFAULT_ACCESSER_STATIC_VARIABLE(type, name, Name)\
private:\
    static type name;\
public:\
    static type get##Name() {return name;}\
    static void set##Name(const type& value) {name = value; saveToFile();}

namespace bumbleBee {
/// @brief 모든 설정은 이 객체를 통해 스태틱하게 제공됨.
class SettingsManager {
    /// @brief 봇 토큰
    _DECLARE_DEFAULT_ACCESSER_STATIC_VARIABLE(std::string, TOKEN, TOKEN)
    /// @brief yt-dlp 실행 명령어
    _DECLARE_DEFAULT_ACCESSER_STATIC_VARIABLE(std::string, YTDLP_CMD, YTDLP_CMD)
    /// @brief ffmpeg 실행 명령어
    _DECLARE_DEFAULT_ACCESSER_STATIC_VARIABLE(std::string, FFMPEG_CMD, FFMPEG_CMD)
    /// @brief 로그레벨
    _DECLARE_DEFAULT_ACCESSER_STATIC_VARIABLE(dpp::loglevel, LOGLEVEL, LOGLEVEL)
    /// @brief 이전에 있던 명령을 지우고 등록할지 선택합니다.
    _DECLARE_DEFAULT_ACCESSER_STATIC_VARIABLE(bool, REGISTER_COMMAND, REGISTER_COMMAND)
public:
    /// @brief 설정 로드하기, 설정은 이 load()를 부르기 전까지는 적절하지 못한 값을 가지고 있음.
    /// @return 로드 성공 시 true, 실패 시 false 반환.
    static bool load();
    /// @brief 설정 변경사항 저장
    static void saveToFile();
    /// @brief 토큰이 유효한지 체크합니다.
    /// @return 유효한 토큰이면 true, 아니면 false를 반환합니다.
    static bool validateToken();
};
}