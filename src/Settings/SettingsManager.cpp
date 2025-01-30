#include <Settings/SettingsManager.hpp>

namespace bumbleBee {

std::string settingsManager::TOKEN = "";
std::string settingsManager::YTDLP_CMD = "./yt-dlp";
std::string settingsManager::FFMPEG_CMD = "./ffmpeg/bin/ffmpeg";
std::string settingsManager::DBURL = "";
std::string settingsManager::DBUSER = "";
std::string settingsManager::DBPASSWORD = "";

void settingsManager::load() {
    nlohmann::json configdocument;
    try {
        std::ifstream configfile("config.json");
        configfile >> configdocument;

        TOKEN = configdocument["TOKEN"];
        YTDLP_CMD = configdocument["YTDLP_CMD"];
        FFMPEG_CMD = configdocument["FFMPEG_CMD"];
        DBURL = configdocument["DBURL"];
        DBUSER = configdocument["DBUSER"];
        DBPASSWORD = configdocument["DBPASSWORD"];
    }
    catch (const nlohmann::json::type_error& e) {
        saveToFile();
        std::ifstream configfile("config.json");
        configfile >> configdocument;

        TOKEN = configdocument["TOKEN"];
        YTDLP_CMD = configdocument["YTDLP_CMD"];
        FFMPEG_CMD = configdocument["FFMPEG_CMD"];
        DBURL = configdocument["DBURL"];
        DBUSER = configdocument["DBUSER"];
        DBPASSWORD = configdocument["DBPASSWORD"];
    }
    catch (const nlohmann::json::parse_error& e) {
        saveToFile();
        std::ifstream configfile("config.json");
        configfile >> configdocument;

        TOKEN = configdocument["TOKEN"];
        YTDLP_CMD = configdocument["YTDLP_CMD"];
        FFMPEG_CMD = configdocument["FFMPEG_CMD"];
        DBURL = configdocument["DBURL"];
        DBUSER = configdocument["DBUSER"];
        DBPASSWORD = configdocument["DBPASSWORD"];
    }
}

void settingsManager::saveToFile() {
    nlohmann::json configdocument;

    configdocument["TOKEN"] = TOKEN;
    configdocument["YTDLP_CMD"] = YTDLP_CMD;
    configdocument["FFMPEG_CMD"] = FFMPEG_CMD;
    configdocument["DBURL"] = DBURL;
    configdocument["DBUSER"] = DBUSER;
    configdocument["DBPASSWORD"] = DBPASSWORD;

    std::ofstream configFile("config.json");
    if (configFile.is_open()) {
        configFile << configdocument.dump(4);
        configFile.close();
    }
}
}