#include <Settings/SettingsManager.hpp>
#include <dpp/nlohmann/json.hpp>
#include <Utils/ConsoleUtils.hpp>
#include <fstream>
#include <sstream>

namespace bumbleBee {

std::string SettingsManager::TOKEN = "";
std::string SettingsManager::YTDLP_CMD = "./yt-dlp";
std::string SettingsManager::FFMPEG_CMD = "./ffmpeg/bin/ffmpeg";
dpp::loglevel SettingsManager::LOGLEVEL = dpp::ll_debug;
bool SettingsManager::REGISTER_COMMAND = false;

bool SettingsManager::validateToken() {
    nlohmann::json response;
    if (ConsoleUtils::getResultFromCommand("which curl").size() == 0) {
        std::cout << "curl is unavaliable. unresolable error please install curl." << std::endl;
        return false;
    }

    std::string stresult = ConsoleUtils::getResultFromCommand("curl -sX GET \"https://discord.com/api/v10/users/@me\" -H \"Authorization: Bot " +
        TOKEN + "\"").front();
    std::stringstream ss(stresult);
    ss >> response;

    if (response.contains("message") && response["message"] == "401: Unauthorized") {
        std::cout << "Token is invalid" << std::endl;
        return false;
    }
    else
        return true;
}

bool SettingsManager::load() {
    nlohmann::json configdocument;
    try {
        std::ifstream configfile("config.json");
        if (!configfile) {
            saveToFile();
            return false;
        }
        configfile >> configdocument;

        TOKEN = configdocument["TOKEN"];

        if (!validateToken())
            return false;

        YTDLP_CMD = configdocument["YTDLP_CMD"];
        FFMPEG_CMD = configdocument["FFMPEG_CMD"];

        std::string level = configdocument["LOGLEVEL"];
        
        std::transform(level.begin(), level.end(), level.begin(), ::tolower);
        if (level == "trace")
            LOGLEVEL = dpp::ll_trace;
        else if (level == "debug")
            LOGLEVEL = dpp::ll_debug;
        else if (level == "warning")
            LOGLEVEL = dpp::ll_warning;
        else if (level == "error")
            LOGLEVEL = dpp::ll_error;
        else if (level == "critical")
            LOGLEVEL = dpp::ll_critical;
        else // 값이 병신같을때 기본값으로 ll_info 부여
            LOGLEVEL = dpp::ll_info;

        REGISTER_COMMAND = configdocument["CLEAR_PREVIOUS_COMMAND"];
    }
    catch (const nlohmann::json::type_error& e) {
        saveToFile();
        return load();
    }
    catch (const nlohmann::json::parse_error& e) {
        saveToFile();
        return load();
    }
    return true;
}

void SettingsManager::saveToFile() {
    nlohmann::json configdocument;

    configdocument["TOKEN"] = TOKEN;
    configdocument["YTDLP_CMD"] = YTDLP_CMD;
    configdocument["FFMPEG_CMD"] = FFMPEG_CMD;

    switch (LOGLEVEL) {
    case dpp::ll_trace:
        configdocument["LOGLEVEL"] = "trace";
        break;
    case dpp::ll_debug:
        configdocument["LOGLEVEL"] = "debug";
        break;
    case dpp::ll_info:
        configdocument["LOGLEVEL"] = "info";
        break;
    case dpp::ll_warning:
        configdocument["LOGLEVEL"] = "warning";
        break;
    case dpp::ll_error:
        configdocument["LOGLEVEL"] = "error";
        break;
    default:
        configdocument["LOGLEVEL"] = "critical";
        break;
    }
    
    configdocument["CLEAR_PREVIOUS_COMMAND"] = REGISTER_COMMAND;

    std::ofstream configFile("config.json");
    if (configFile.is_open()) {
        configFile << configdocument.dump(4);
        configFile.close();
    }
}
}