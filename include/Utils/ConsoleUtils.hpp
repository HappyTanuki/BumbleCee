#pragma once
#include <iostream>
#include <sstream>
#include <queue>
#include <regex>
#include <boost/process.hpp>

namespace bumbleBee {
class ConsoleUtils {
public:
    /** 
     * @brief 명령어를 쉘에서 실행하고 결과를 EOF 전까지 읽어 \n을 구분자로 토큰화하여 반환합니다
     * @param cmd 실행할 명령
     * @param args 아규먼트
     * @return std::queue<std::string> tokens
     */
    static std::queue<std::string> safe_execute_command(const std::string& cmd, const std::vector<std::string>& args) {
        std::queue<std::string> tokens;
        try {
            boost::process::ipstream output;  // 명령의 출력을 받을 스트림
            boost::process::child c(cmd, boost::process::args(args), boost::process::std_out > output);

            std::string line;
            while (!output.eof() && std::getline(output, line))
                tokens.push(line);

            c.wait();  // 프로세스가 종료될 때까지 대기
            return tokens;
        } catch (const std::exception& e) {
            return tokens;
        }
    }
    /** 
     * @brief 명령어를 쉘에서 실행하고 결과를 파이프로 연결하여 반환합니다
     * @param cmd 실행할 명령
     * @param args 아규먼트
     * @return FILE* fd
     */
    static FILE* safe_open_pipe(const std::string& cmd, const std::vector<std::string>& args) {
        boost::process::pipe pipe;
        boost::process::child c(cmd, boost::process::args(args), boost::process::std_out > pipe);

        return fdopen(pipe.native_source(), "r");
    }
};
}