#pragma once
#include <string>
#include <dpp/dpp.h>

struct FQueueElement {
    std::string ID;
    dpp::embed embed;
    bool skip = false;
};