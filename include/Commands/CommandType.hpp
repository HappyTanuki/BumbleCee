#pragma once
#include <dpp/dpp.h>
#include <vector>
#include <list>
#include <MusicQueue.hpp>

namespace commands {
class ICommand {
public:
    //이 생성자를 명시적으로 호출할 것.
    ICommand(std::shared_ptr<dpp::cluster> botCluster);
    virtual void operator()(const dpp::slashcommand_t &event) = 0;

    std::vector<dpp::slashcommand> commandObjectVector;
protected:
    std::shared_ptr<dpp::cluster> botCluster;
};
}