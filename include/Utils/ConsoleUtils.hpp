#pragma once
#ifndef _CONSOLEUTILS_HPP_
#define _CONSOLEUTILS_HPP_
#include <iostream>
#include <sstream>
#include <queue>

namespace bumbleBee {
class ConsoleUtils {
public:
    /** 
     * @brief 명령어를 쉘에서 실행하고 결과를 EOF 전까지 읽어 \n을 구분자로 토큰화하여 반환합니다
     * @param cmd 실행할 명령
     * @return std::queue<std::string> tokens
     */
    static std::queue<std::string> getResultFromCommand(std::string cmd) {
        std::string result, token;
        std::queue<std::string> tokens;
        FILE* stream;
        const int maxBuffer = 12; // 적당한 크기
        char buffer[maxBuffer];
        cmd.append(" 2>&1"); // 표준에러를 표준출력으로 redirect

        stream = popen(cmd.c_str(), "r"); // 주어진 command를 shell로 실행하고 파이프 연결 (fd 반환)
            if (stream) {
                while (fgets(buffer, maxBuffer, stream) != NULL) result.append(buffer); // fgets: fd (stream)를 길이 (maxBuffer)만큼 읽어 버퍼 (buffer)에 저장
                pclose(stream);
            }

        std::stringstream ss(result);
        while (std::getline(ss, token, '\n')) {
            tokens.push(token);
        }

        return tokens;
    }
};
}

#endif