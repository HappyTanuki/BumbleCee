#pragma once
#include <Commands/CommandType.hpp>
#include <BumbleCeepp.hpp>
#include <memory>

namespace commands {
class Play : public ICommand {
public:
    Play(dpp::snowflake botID, BumbleCeepp* Bot);

    void operator()(const dpp::slashcommand_t& event);
    void on_DLCompleted(std::string musicID, dpp::embed embed, const dpp::slashcommand_t& event);
};
}