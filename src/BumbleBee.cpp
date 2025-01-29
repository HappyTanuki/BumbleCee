#include <BumbleBee.hpp>

namespace bumbleBee{
BumbleBee::BumbleBee(nlohmann::json settings) {
    this->cluster = std::make_shared<dpp::cluster>(settings["token"]);
    dbDriver = sql::mariadb::get_driver_instance();
    this->dbURL = std::make_shared<sql::SQLString>(settings["dbURL"]);
    sql::Properties pro({
        {"user", std::string(settings["user"])},
        {"password", std::string(settings["password"])}
    });
    this->dbProperties = std::make_shared<sql::Properties>(pro);

    cluster->on_log(dpp::utility::cout_logger());
    cluster->on_slashcommand([this](const dpp::slashcommand_t& event){on_slashcommand(event);});
    cluster->on_ready([this](const dpp::ready_t &event){on_ready(event);});

    queue = std::make_shared<MusicQueue>();
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