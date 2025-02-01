#pragma once
#ifndef _MUSICQUEUEELEMENT_HPP_
#define _MUSICQUEUEELEMENT_HPP_
#include <dpp/dpp.h>

namespace bumbleBee {

class MusicQueueElement {
public:
    MusicQueueElement(
        std::string id,
        std::string query,
        dpp::user issuingUser,
        dpp::embed embed) :
        id(id), query(query), issuingUser(issuingUser), embed(embed) {}

    const std::string id;
    const std::string query;
    const dpp::user issuingUser;
    const dpp::embed embed;
};
}
#endif