#ifndef BUMBLEBEE_INCLUDE_UTILS_UPDATE_CHECKER_H_
#define BUMBLEBEE_INCLUDE_UTILS_UPDATE_CHECKER_H_

#include "precomp.h"

namespace utils {

int InstallYtdlp(boost::asio::io_context& ctx);

int CheckUpdate(boost::asio::io_context& ctx);

}  // namespace utils

#endif