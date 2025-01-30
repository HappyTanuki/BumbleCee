#include <BumbleBee.hpp>
#include <memory>
#include <Settings/SettingsManager.hpp>
#include <Utils/VersionsCheckUtils.hpp>

namespace bumbleBee{
BumbleBee::BumbleBee() {
    settingsManager::load();
    this->cluster = std::make_shared<dpp::cluster>(settingsManager::TOKEN);
    dbDriver = sql::mariadb::get_driver_instance();
    this->dbURL = std::make_shared<sql::SQLString>(settingsManager::DBURL);
    sql::Properties pro({
        {"user", std::string(settingsManager::DBUSER)},
        {"password", std::string(settingsManager::DBPASSWORD)}
    });
    this->dbProperties = std::make_shared<sql::Properties>(pro);

    cluster->on_log(dpp::utility::cout_logger());
    cluster->on_slashcommand([this](const dpp::slashcommand_t& event){on_slashcommand(event);});
    cluster->on_ready([this](const dpp::ready_t &event){on_ready(event);});

    queue = std::make_shared<MusicQueue>();
    
    VersionsCheckUtils::validateYTDLPFFMPEGBinary(cluster);
    VersionsCheckUtils::updateytdlp(cluster);
}

void BumbleBee::start() { this->cluster->start(dpp::st_wait); }

void BumbleBee::on_slashcommand(const dpp::slashcommand_t &event) {
    for (auto command : commands)
        for (auto alias : command->nameAndAliases)
            if (event.command.get_command_name() == alias.name)
                (*command)(event);
}

void BumbleBee::on_ready(const dpp::ready_t &event) {
    cluster->log(dpp::loglevel::ll_info, "Bot ready.");
}
}