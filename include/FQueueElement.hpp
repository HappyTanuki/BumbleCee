#pragma once
#include <string>
#include <dpp/dpp.h>

struct FQueueElement {
    std::string URL;
    std::string title;
    std::string description;
    std::string fileName;
    std::string thumbnail;
    std::string duration;
    dpp::embed embed;
};