#ifndef BUMBLEBEE_INCLUDE_UTILS_CONSOLE_H_
#define BUMBLEBEE_INCLUDE_UTILS_CONSOLE_H_

#include "precomp.h"

namespace utils {
// @brief 명령어가 실행 가능한지 체크합니다
// @param cmd 실행할 명령
// @return int error_code
int ValidateCommand(std::string cmd);
// @brief 명령어를 쉘에서 실행하고 결과를 EOF 전까지 읽어 반환합니다
// @param cmd 실행할 명령
// @param args 아규먼트
// @return int error_code
int ExecuteCommand(
    const std::string& cmd,
    const std::vector<std::string>& args = std::vector<std::string>());
// @brief 명령어를 쉘에서 실행하고 결과를 EOF 전까지 읽어 반환합니다
// @param cmd 실행할 명령
// @param result 실행결과
// @return int error_code
int ExecuteCommand(const std::string& cmd, std::string& result);
// @brief 명령어를 쉘에서 실행하고 결과를 EOF 전까지 읽어 반환합니다
// @param cmd 실행할 명령
// @param args 아규먼트
// @param result 실행결과
// @return int error_code
int ExecuteCommand(const std::string& cmd, const std::vector<std::string>& args,
                   std::string& result);
// @brief 명령어를 쉘에서 실행하고 결과를 파이프로 연결하여 반환합니다
// @param cmd 실행할 명령
// @param args 아규먼트
// @return boost::process::popen
boost::process::popen OpenPipe(
    const std::string& cmd,
    const std::vector<std::string>& args = std::vector<std::string>());
}  // namespace utils
#endif