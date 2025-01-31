#pragma once
#ifndef _BUMBLEBEE_HPP_
#define _BUMBLEBEE_HPP_
#include <dpp/dpp.h>
#include <dpp/nlohmann/json.hpp>
#include <memory>
#include <Commands/BumbleBeeCommand.hpp>

namespace bumbleBee {
/**
 * @file BumbleBee.hpp
 * @brief 메인 봇 클래스
**/
class BumbleBee {
public:
    /**
     * @brief 생성자
    **/
    BumbleBee();
    /**
     * @brief 파괴자
     * @details BumbleBee의 모든 Property를 책임지고 파괴합니다
    **/
    ~BumbleBee() {}

    /**
     * @brief 봇 시작
    **/
    void start();

    /**
     * @brief slashcommand 이벤트 인지시 콜백되는 함수
     * @param event const dpp::slashcommand_t&
    **/
    void on_slashcommand(const dpp::slashcommand_t& event);
    /**
     * @brief ready 이벤트 인지시 콜백되는 함수
     * @param event const dpp::ready_t&
    **/
    void on_ready(const dpp::ready_t& event);

    /// @brief DPP 기본 클러스터 객체
    std::shared_ptr<dpp::cluster> cluster;
private:
    /// @brief Command 목록
    std::unordered_map<std::string, std::shared_ptr<commands::ICommand>> commands;
    /// @brief voiceclient 관련 event 처리기 <guild id, 각 길드별 MusicPlayManager 인스턴스>
    std::shared_ptr<MusicPlayManager> musicManager;
};
}
#endif