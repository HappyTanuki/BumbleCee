#ifndef BUMBLEBEE_INCLUDE_UTILS_FILE_DOWNLOADER_H_
#define BUMBLEBEE_INCLUDE_UTILS_FILE_DOWNLOADER_H_
#include "precomp.h"

namespace utils {

void DownloadFileFromHTTPS(std::string url, std::string filename);
}

#endif