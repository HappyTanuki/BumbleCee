#include "precomp.h"
#include "utils/console.h"
#include "utils/update_checker.h"

int main() {
  boost::asio::io_context ctx;
  boost::system::error_code ec;

  utils::CheckUpdate(ctx);

  char buf[8192];
#ifdef WIN32
  /*try {
          auto ytdlp_pipe = utils::OpenPipe(ctx,
                  "yt-dlp.exe", { "-o", "-", "--quiet", "--ignore-errors", "-f",
  "bestaudio", "https://youtu.be/9_bTl2vvYQg?si=IVhvpDhnpPvziwQR" });

          while (true) {
                  boost::system::error_code read_ec;
                  size_t bytes_read =
                          boost::asio::read(ytdlp_pipe, boost::asio::buffer(buf,
  8192), read_ec); if (bytes_read > 0) { std::cout.write(buf, bytes_read);
                  }
                  if (read_ec == boost::asio::error::eof || read_ec) {
                          break;
                  }
          }
  }
  catch (const boost::process::system_error& e) {
          std::string error = e.what();
          return -1;
  }*/
#else
  auto ytdlp_pipe = utils::OpenPipe(
      ctx, "yt-dlp",
      {"-o", "-", "--quiet", "--ignore-errors", "-f", "bestaudio",
       "https://youtu.be/9_bTl2vvYQg?si=IVhvpDhnpPvziwQR"});

  while (true) {
    boost::system::error_code read_ec;
    size_t bytes_read =
        boost::asio::read(ytdlp_pipe, boost::asio::buffer(buf, 8192), read_ec);
    if (bytes_read > 0) {
      std::cout.write(buf, bytes_read);
    }
    if (read_ec == boost::asio::error::eof || read_ec) {
      break;
    }
  }
#endif
  return 0;
}