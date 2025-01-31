#include <BumbleBee.hpp>
#include <memory>
#include <Settings/SettingsManager.hpp>
#include <Utils/VersionsCheckUtils.hpp>
#include <Audio/MusicPlayManager.hpp>
#include <Commands/BumbleBeeCommand.hpp>

namespace bumbleBee{
BumbleBee::BumbleBee() {
    if (!settingsManager::load()) {
        std::cout << "Please configure confing.json" << std::endl;
        exit(1);
    }

    cluster = std::make_shared<dpp::cluster>(settingsManager::getTOKEN());

    cluster->on_log([](const dpp::log_t& event) {
		if (event.severity >= settingsManager::getLOGLEVEL()) {
			std::cout << "[" << dpp::utility::current_date_time() << "] " << dpp::utility::loglevel(event.severity) << ": " << event.message << "\n";
		}
	});
    cluster->on_slashcommand([this](const dpp::slashcommand_t& event){on_slashcommand(event);});
    cluster->on_ready([this](const dpp::ready_t &event){on_ready(event);});

    VersionsCheckUtils::validateYTDLPFFMPEGBinary(cluster);
    VersionsCheckUtils::updateytdlp(cluster);

    musicManager = std::make_shared<MusicPlayManager>(cluster);

    commands["d"] = std::make_shared<commands::Delete>  (cluster->cluster_id, musicManager);
    commands["l"] = std::make_shared<commands::Leave>   (cluster->cluster_id, musicManager);
    commands["p"] = std::make_shared<commands::Play>    (cluster->cluster_id, musicManager);
    commands["q"] = std::make_shared<commands::Queue>   (cluster->cluster_id, musicManager);
    commands["r"] = std::make_shared<commands::Repeat>  (cluster->cluster_id, musicManager);
    commands["s"] = std::make_shared<commands::Skip>    (cluster->cluster_id, musicManager);

    for (auto command : commands) {
        for (auto aliase : command.second->aliases) {
            commands[aliase] = std::shared_ptr<commands::ICommand>(command.second);
            commands[aliase]->set_name(aliase);
        }
    }
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
    cluster->log(dpp::loglevel::ll_info, "Bot ready.");
    
    if (event.guilds.size() == 0) {
        cluster->log(dpp::loglevel::ll_info, "Bot is not on any server! Please invite this bot to any server.");
        exit(1);
    }
    
    if (dpp::run_once<struct register_bot_commands>()) {
        if (settingsManager::getCLCOMMAND()) {
            cluster->log(dpp::loglevel::ll_info, "Clear Pre-installed commands");
            cluster->global_bulk_command_delete([](const dpp::confirmation_callback_t &t){
                std::cout << "cleared Pre-installed commands. Please restart Bot" << std::endl;
                settingsManager::setCLCOMMAND(false);
                exit(0);
            });
            return;
        }
        for (auto command : commands)
            cluster->global_command_create(*command.second);
    }
    cluster->log(dpp::loglevel::ll_info, "Command created.");
}
}