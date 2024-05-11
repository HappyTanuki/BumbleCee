#pragma once
#include <dpp/dpp.h>
#include <vector>
#include <list>

class BumbleCeepp;

namespace commands {
class ICommand {
public:
    ICommand(dpp::snowflake botID, BumbleCeepp* Bot)
    {
        this->botID = botID;
        this->Bot = Bot;
    }
    virtual void operator()(const dpp::slashcommand_t &event) = 0;

    std::vector<dpp::slashcommand> commandObjectVector;
    dpp::snowflake botID;
    BumbleCeepp* Bot;
};
}