#ifndef _COMMANDTYPE_HPP_
#define _COMMANDTYPE_HPP_
#include <dpp/dpp.h>
#include <vector>
#include <list>
#include <FQueueElement.hpp>

class ICommand {
public:
    virtual void operator() (const dpp::slashcommand_t& Event) = 0;
    virtual void operator() (std::list<FQueueElement>& MusicQueue, const dpp::slashcommand_t& Event) = 0;

    std::vector<dpp::slashcommand> CommandObjectVector;
};

#endif