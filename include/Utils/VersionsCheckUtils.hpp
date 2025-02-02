#pragma once
#include <dpp/dpp.h>
#include "ConsoleUtils.hpp"
#include "../Settings/SettingsManager.hpp"

namespace bumbleBee {
class VersionsCheckUtils {
public:
    static bool isThereCMD(std::shared_ptr<dpp::cluster> cluster, std::string cmd) {
        if (ConsoleUtils::getResultFromCommand("which " + cmd).size() == 0) {
            cluster->log(dpp::ll_error, cmd + " is unavaliable. unresolable error please install " + cmd);
            return false;
        }
        else
            return true;
    }

    static void validateYTDLPFFMPEGBinary(std::shared_ptr<dpp::cluster> cluster) {
        cluster->log(dpp::ll_info, "Checking if yt-dlp and ffmpeg is available...");
        std::queue<std::string> result = ConsoleUtils::getResultFromCommand(SettingsManager::getFFMPEG_CMD() + " -version");
        std::string front = result.front();
        if (front[0] != 'f' ||
            front[1] != 'f' ||
            front[2] != 'm' ||
            front[3] != 'p' ||
            front[4] != 'e' ||
            front[5] != 'g') {
            cluster->log(dpp::ll_warning, "ffmpeg is unavailable. downloading ffmpeg...");

            if (!isThereCMD(cluster, "curl")) {
                exit(1);
            }
            if (!isThereCMD(cluster, "tar")) {
                exit(1);
            }

            system("curl -LO https://github.com/BtbN/FFmpeg-Builds/releases/latest/download/ffmpeg-master-latest-linux64-gpl.tar.xz");
            system("tar -xf ffmpeg-master-latest-linux64-gpl.tar.xz");
            system("rm ffmpeg-master-latest-linux64-gpl.tar.xz");
            system("mv ffmpeg-master-latest-linux64-gpl ffmpeg");
            SettingsManager::setFFMPEG_CMD("./ffmpeg/bin/ffmpeg");
        }
        result = ConsoleUtils::getResultFromCommand(SettingsManager::getYTDLP_CMD() + " --version");
        front = result.front();
        if ((front[0]-'0' < 0 || front[0]-'0' > 9) ||
            (front[1]-'0' < 0 || front[1]-'0' > 9) ||
            (front[2]-'0' < 0 || front[2]-'0' > 9) ||
            (front[3]-'0' < 0 || front[3]-'0' > 9)) {
            cluster->log(dpp::ll_warning, "ytdlp is unavailable. downloading ytdlp...");

            if (!isThereCMD(cluster, "curl")) {
                exit(1);
            }
            if (!isThereCMD(cluster, "tar")) {
                exit(1);
            }

            system("curl -LO https://github.com/yt-dlp/yt-dlp/releases/latest/download/yt-dlp");
            system("chmod +x ./yt-dlp");
            SettingsManager::setYTDLP_CMD("./yt-dlp");
        }
    }

    static void updateytdlp(std::shared_ptr<dpp::cluster> cluster) {
        cluster->log(dpp::ll_info, "Checking if yt-dlp update is available...");
        std::queue<std::string> result = ConsoleUtils::getResultFromCommand("./yt-dlp -U");
        while(!result.empty()) {
            std::string front = result.front();
            result.pop();
            cluster->log(dpp::ll_info, front);
        }
    }
};
}