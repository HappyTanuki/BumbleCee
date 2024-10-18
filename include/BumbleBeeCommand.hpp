#pragma once
#ifndef _BUMBLEBEECOMMAND_HPP_
#define _BUMBLEBEECOMMAND_HPP_
#include <dpp/dpp.h>
#include <MusicQueue.hpp>

namespace bumbleBee::commands {
class ICommand {
public:
    /// @brief 기본 생성자
    /// @param botID 명령어 등록을 위한 봇 아이디
    /// @param queue 음악이 저장되어 있는 큐
    ICommand(dpp::snowflake botID, std::weak_ptr<MusicQueue> queue) {
        this->botID = botID;
        this->queue = queue;
    }
    /// @brief 명령어 호출 시에 콜백되는 함수
    /// @param event
    virtual void operator()(const dpp::slashcommand_t &event){};

    /// @brief 봇 명령어들의 이름과 별명들을 저장하는 벡터
    std::vector<dpp::slashcommand> nameAndAliases;

private:
    /// @brief 봇 ID
    dpp::snowflake botID;
    /// @brief 음악 큐에 대한 약한 포인터
    std::weak_ptr<MusicQueue> queue;
};
}

/**
 * @fn _DECLARE_BUMBLEBEE_COMMAND_one_PARAM_one_ALIAS(name, alias, description, option_name, option_desc)
 * @brief 명령어 인자가 없는 명령어의 boilerplate 대체 매크로
 * @param name 명령어 이름 및 클래스명
 * @param alias 명령어 별명
 * @param description 명령어 설명
**/
#define _DECLARE_BUMBLEBEE_COMMAND_one_ALIAS(name, alias, description) \
namespace bumbleBee::commands { \
class name : public ICommand { \
public: \
    name (dpp::snowflake botID, std::weak_ptr<MusicQueue> q) \
    : ICommand(botID, q) { \
        dpp::slashcommand command = dpp::slashcommand(#name, description, botID); \
        dpp::slashcommand aliasCommand = dpp::slashcommand(alias, description, botID); \
        nameAndAliases.push_back(command); \
        nameAndAliases.push_back(aliasCommand); \
    } \
    virtual void operator()(const dpp::slashcommand_t &event) override; \
}; \
}

/**
 * @fn _DECLARE_BUMBLEBEE_COMMAND_one_PARAM_one_ALIAS(name, alias, description, option_name, option_desc)
 * @brief 명령어 인자를 하나 갖는 명령어의 boilerplate 대체 매크로
 * @param name 명령어 이름 및 클래스명
 * @param alias 명령어 별명
 * @param description 명령어 설명
 * @param option_name 명령어 인자 이름
 * @param option_desc 명령어 인자 설명
**/
#define _DECLARE_BUMBLEBEE_COMMAND_one_PARAM_one_ALIAS(name, alias, description, option_name, option_desc) \
namespace bumbleBee::commands { \
class name : public ICommand { \
public: \
    name (dpp::snowflake botID, std::weak_ptr<MusicQueue> q) \
    : ICommand(botID, q) { \
        dpp::slashcommand command = dpp::slashcommand(#name, description, botID); \
        dpp::slashcommand aliasCommand = dpp::slashcommand(alias, description, botID); \
        command.add_option( \
            dpp::command_option(dpp::co_string, option_name, option_desc, botID) \
        ); \
        aliasCommand.add_option( \
            dpp::command_option(dpp::co_string, option_name, option_desc, botID) \
        ); \
        nameAndAliases.push_back(command); \
        nameAndAliases.push_back(aliasCommand); \
    } \
    virtual void operator()(const dpp::slashcommand_t &event) override; \
}; \
}

_DECLARE_BUMBLEBEE_COMMAND_one_PARAM_one_ALIAS(Delete, "d", "큐의 해당하는 번호의 노래를 지웁니다", "pos", "큐 번호")
_DECLARE_BUMBLEBEE_COMMAND_one_ALIAS(Leave, "l", "음성 채팅방을 떠납니다")
_DECLARE_BUMBLEBEE_COMMAND_one_PARAM_one_ALIAS(Play, "p", "노래 재생", "query", "링크 또는 검색어")
_DECLARE_BUMBLEBEE_COMMAND_one_ALIAS(Queue, "q", "노래 예약 큐를 출력합니다")
_DECLARE_BUMBLEBEE_COMMAND_one_ALIAS(Repeat, "r", "반복을 켜거나 끕니다")
_DECLARE_BUMBLEBEE_COMMAND_one_ALIAS(Skip, "s", "현재 재생중인 곡을 스킵합니다")

#endif