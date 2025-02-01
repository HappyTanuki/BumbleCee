#include <BumbleBee.hpp>
#include <memory>
#include <Settings/SettingsManager.hpp>
#include <Utils/VersionsCheckUtils.hpp>
#include <Audio/MusicPlayManager.hpp>
#include <Commands/BumbleBeeCommand.hpp>

namespace bumbleBee{
BumbleBee::BumbleBee() {
    if (!SettingsManager::load()) {
        std::cout << "Please configure confing.json" << std::endl;
        exit(1);
    }

    cluster = std::make_shared<dpp::cluster>(SettingsManager::getTOKEN());

    cluster->on_log([](const dpp::log_t& event) {
		if (event.severity >= SettingsManager::getLOGLEVEL()) {
			std::cout << "[" << dpp::utility::current_date_time() << "] " << dpp::utility::loglevel(event.severity) << ": " << event.message << std::endl;
		}
	});
    cluster->on_slashcommand([this](const dpp::slashcommand_t& event){on_slashcommand(event);});
    cluster->on_ready([this](const dpp::ready_t &event){on_ready(event);});

    VersionsCheckUtils::validateYTDLPFFMPEGBinary(cluster);
    VersionsCheckUtils::updateytdlp(cluster);
}

void BumbleBee::start() { this->cluster->start(dpp::st_wait); }

void BumbleBee::on_slashcommand(const dpp::slashcommand_t &event) {
    if (commands.find(event.command.get_command_name()) != commands.end()) {
        event.thinking();
        auto command = commands.at(event.command.get_command_name());
        std::thread t([](const dpp::slashcommand_t &event, std::shared_ptr<commands::ICommand> command){
            command->execute(event);
        }, event, command);
        t.detach();
    }
}

void BumbleBee::on_ready(const dpp::ready_t &event) {
    GIDs = event.guilds;
    musicManager = std::make_shared<MusicPlayManager>(cluster, GIDs);

    commands["d"]       = std::make_shared<commands::Delete>  (cluster->cluster_id, musicManager);
    commands["l"]       = std::make_shared<commands::Leave>   (cluster->cluster_id, musicManager);
    commands["p"]       = std::make_shared<commands::Play>    (cluster->cluster_id, musicManager);
    commands["q"]       = std::make_shared<commands::Queue>   (cluster->cluster_id, musicManager);
    commands["r"]       = std::make_shared<commands::Repeat>  (cluster->cluster_id, musicManager);
    commands["s"]       = std::make_shared<commands::Skip>    (cluster->cluster_id, musicManager);
    commands["shuffle"] = std::make_shared<commands::Shuffle> (cluster->cluster_id, musicManager);
    
    if (event.guilds.size() == 0) {
        cluster->log(dpp::loglevel::ll_info, "Bot is not on any server! Please invite this bot to any server.");
        exit(1);
    }
    
    if (dpp::run_once<struct register_bot_commands>()) {
        if (SettingsManager::getREGISTER_COMMAND()) {
            cluster->log(dpp::loglevel::ll_info, "Clear Pre-installed commands");

            cluster->global_bulk_command_create({
                *commands["d"],
                *commands["l"],
                *commands["p"],
                *commands["q"],
                *commands["r"],
                *commands["s"],
                *commands["shuffle"]
            }, [&](const dpp::confirmation_callback_t &t){
                cluster->log(dpp::loglevel::ll_info, "Command created.");
                SettingsManager::setREGISTER_COMMAND(false);
            });
        }
    }

    cluster->log(dpp::loglevel::ll_info, "Bot ready.");
}
}