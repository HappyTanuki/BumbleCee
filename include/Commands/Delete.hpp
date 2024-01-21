#pragma once
#include <Commands/CommandType.hpp>
#include <BumbleCeepp.hpp>
#include <memory>

namespace commands {
class Delete : public VCCommand {
public:
    Delete(std::shared_ptr<dpp::cluster> botCluster, std::unordered_map<dpp::snowflake, std::shared_ptr<MusicQueue>> *queueMap);

    void operator()(const dpp::slashcommand_t& event);
};
}