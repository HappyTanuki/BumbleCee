#pragma once
#ifndef _MUSICPLAYMANAGER_HPP_
#define _MUSICPLAYMANAGER_HPP_
#include <dpp/dpp.h>

namespace BumbleBee {
class MusicPlayManager {
public:
    MusicPlayManager(std::shared_ptr<dpp::cluster> cluster) {
        this->cluster = cluster;
    }
private:
    std::shared_ptr<dpp::cluster> cluster;
};
}
#endif