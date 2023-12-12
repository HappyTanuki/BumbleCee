#pragma once
#include "../CommandType.hpp"

class Join : public ICommand {
public:
    Join(dpp::snowflake Id);

    void operator()(const dpp::slashcommand_t& Event) {}
    void operator()(std::list<std::string>& MusicQueue, const dpp::slashcommand_t& Event);
};