#pragma once
#include <Commands/CommandType.hpp>
#include <BumbleCeepp.hpp>
#include <memory>

namespace commands {
class Delete : public ICommand {
public:
    Delete(dpp::snowflake botID, BumbleCeepp* Bot);

    void operator()(const dpp::slashcommand_t& event);
};
}