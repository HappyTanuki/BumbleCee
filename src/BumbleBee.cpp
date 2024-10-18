#include <BumbleBee.hpp>

namespace bumbleBee{
BumbleBee::BumbleBee(nlohmann::json settings) {
    this->cluster = std::make_unique<dpp::cluster>(settings["token"]);
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
}

BumbleBee::~BumbleBee() {
}

void BumbleBee::start() { this->cluster->start(dpp::st_wait); }

bool BumbleBee::addCommand(commands::ICommand cmd) {
    commands.push_back(cmd);
    return false;
}

void BumbleBee::on_slashcommand(const dpp::slashcommand_t &event) {

}
void BumbleBee::on_ready(const dpp::ready_t &event) {
    cluster->log(dpp::loglevel::ll_info, "Bot ready.");
}

}