#ifndef _FQUEUEELEMENT_HPP_
#define _FQUEUEELEMENT_HPP_
#include <string>
#include <dpp/dpp.h>

struct FQueueElement {
    std::string URL;
    std::string title;
    std::string description;
    std::string FileName;
    std::string thumbnail;
    std::string duration;
    dpp::snowflake guild_id;
    dpp::embed embed;
};

#endif