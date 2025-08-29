#include "precomp.h"
#include "utils/console.h"
#include "utils/update_checker.h"

int main(int argc, char* argv[]) {
  boost::system::error_code ec;
  char buf[16384];
  std::string output;

  utils::CheckUpdate();

  // utils::ExecuteCommand("yt-dlp", {"-U"}, output);
  // std::cout << output;

  // auto ytdlp_pipe = utils::OpenPipe(
  //     "yt-dlp", {"-o", "-", "--quiet", "--ignore-errors", "-f", "bestaudio",
  //                "https://youtu.be/9_bTl2vvYQg?si=IVhvpDhnpPvziwQR"});

  // while (true) {
  //   boost::system::error_code read_ec;
  //   size_t bytes_read =
  //       boost::asio::read(ytdlp_pipe, boost::asio::buffer(buf, 16384),
  //       read_ec);

  //   if (bytes_read > 0) {
  //     std::cout.write(buf, bytes_read);
  //   }

  //   if (read_ec == boost::asio::error::eof || read_ec) {
  //     break;
  //   }
  // }

  return 0;
}