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
        dpp::user issuingUser) :
        id(id), query(query), issuingUser(issuingUser) {}

    const std::string id;
    const std::string query;
    const dpp::user issuingUser;
};
}
#endif