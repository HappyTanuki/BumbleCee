#pragma once
#include <dpp/dpp.h>
#include <Audio/MusicPlayManager.hpp>
#include <functional>

namespace bumbleBee::commands {
class ICommand : public dpp::slashcommand {
public:
    /**
     * @brief 기본 생성자
     * @param botID 봇 아이디
     * @param manager 음악재생 매니저
    **/
    ICommand(dpp::snowflake botID, std::shared_ptr<MusicPlayManager> manager) {
        this->botID = botID;
        this->musicManager = manager;
    }
    /**
     * @brief 명령어 호출 시에 콜백될 메소드
     * @param event dpp::slashcommand_t
    **/
    virtual void execute(const dpp::slashcommand_t &event){};

    /// @brief 명령어 별명
    std::vector<std::string> aliases;

private:
    /// @brief 봇 ID
    dpp::snowflake botID;
    
protected:
    /// @brief 음악재생 매니저
    std::shared_ptr<MusicPlayManager> musicManager;

protected:
    /// @brief concrete class에서 구현해야 하는 init 이벤트트
    virtual void init() = 0;
};
}

/**
 * @brief 명령어 인자가 없는 명령어의 boilerplate 대체 매크로
 * @param name 명령어 이름 및 클래스명
 * @param description 명령어 설명
**/
#define _DECLARE_BUMBLEBEE_COMMAND(CLASS, NAME, DESCRIPTION) \
namespace bumbleBee::commands { \
class CLASS : public ICommand { \
public: \
    CLASS (dpp::snowflake botID, std::shared_ptr<MusicPlayManager> manager) \
    : ICommand(botID, manager) { \
        name = #NAME; \
        description = DESCRIPTION; \
        init(); \
    } \
    void execute(const dpp::slashcommand_t &event) override; \
    \
protected: \
    void init() override; \
}; \
}

_DECLARE_BUMBLEBEE_COMMAND(Delete,  d,          "큐의 해당하는 번호의 노래를 지웁니다")
_DECLARE_BUMBLEBEE_COMMAND(Leave,   l,          "음성 채팅방을 떠납니다")
_DECLARE_BUMBLEBEE_COMMAND(Play,    p,          "노래 재생")
_DECLARE_BUMBLEBEE_COMMAND(Queue,   q,          "노래 예약 큐를 확인합니다")
_DECLARE_BUMBLEBEE_COMMAND(Repeat,  r,          "반복을 켜거나 끕니다")
_DECLARE_BUMBLEBEE_COMMAND(Skip,    s,          "현재 재생중인 곡을 스킵합니다")
_DECLARE_BUMBLEBEE_COMMAND(Shuffle, shuffle,    "큐를 섞습니다")