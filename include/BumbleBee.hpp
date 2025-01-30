#pragma once
#ifndef _BUMBLEBEE_HPP_
#define _BUMBLEBEE_HPP_
#include <dpp/dpp.h>
#include <dpp/nlohmann/json.hpp>
#include <memory>
#include <BumbleBeeCommand.hpp>
#include <MusicQueue.hpp>
#include <mariadb/conncpp.hpp>

namespace bumbleBee {
/**
 * @file BumbleBee.hpp
 * @brief 메인 봇 클래스
**/
class BumbleBee {
public:
    /**
     * @fn BumbleBee(nlohmann::json settings)
     * @brief 생성자
    **/
    BumbleBee();
    /**
     * @fn ~BumbleBee()
     * @brief 파괴자
     * @details BumbleBee의 모든 Property를 책임지고 파괴합니다
    **/
    ~BumbleBee() {}

    /**
     * @fn void start()
     * @brief 봇 시작
    **/
    void start();

    /**
     * @fn void on_slashcommand(const dpp::slashcommand_t& event)
     * @brief slashcommand 이벤트 인지시 콜백되는 함수
     * @param event
    **/
    void on_slashcommand(const dpp::slashcommand_t& event);
    /**
     * @fn void on_ready(const dpp::ready_t &event)
     * @brief ready 이벤트 인지시 콜백되는 함수
     * @param event
    **/
    void on_ready(const dpp::ready_t &event);

    /// @brief DPP 기본 클러스터 객체
    std::shared_ptr<dpp::cluster> cluster;
    /// @brief 음악 큐
    std::shared_ptr<MusicQueue> queue;
private:
    /// @brief db 드라이버
    sql::Driver* dbDriver;
    /// @brief db 접속 URL
    std::shared_ptr<sql::SQLString> dbURL;
    /// @brief db 접속 속성
    std::shared_ptr<sql::Properties> dbProperties;
    /// @brief Command 목록
    std::vector<std::shared_ptr<commands::ICommand>> commands;
};
}
#endif