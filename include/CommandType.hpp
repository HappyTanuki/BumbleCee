#pragma once
#include <dpp/dpp.h>
#include <vector>
#include <list>

class ICommand {
public:
    virtual void operator() (const dpp::slashcommand_t& Event) = 0;
    virtual void operator() (std::list<std::string>& MusicQueue, const dpp::slashcommand_t& Event) = 0;

    std::vector<dpp::slashcommand> CommandObjectVector;
};