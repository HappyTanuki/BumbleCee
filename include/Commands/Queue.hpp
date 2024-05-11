#pragma once
#include <Commands/CommandType.hpp>
#include <BumbleCeepp.hpp>
#include <memory>

namespace commands {
class Queue : public ICommand {
public:
    Queue(dpp::snowflake botID, BumbleCeepp* Bot);

    void operator()(const dpp::slashcommand_t& event);
};
}