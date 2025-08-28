#ifdef WIN32
#include <winsock2.h>
#endif
#include <boost/process.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
extern "C" {
#include <libavformat/avformat.h>
}
#include <iostream>

int main(int argc, char* argv[]) {
    std::cout << "FFmpeg version: " << av_version_info() << std::endl;
    return 0;
}