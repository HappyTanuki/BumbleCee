#include "utils/update_checker.h"

#include "precomp.h"
#include "utils/console.h"
#include "utils/file_downloader.h"

namespace utils {

int InstallYtdlp() {
  BOOST_LOG_TRIVIAL(warning) << "ytdlp is unavailable. downloading ytdlp...";
#ifdef WIN32
  DownloadFileFromHTTPS(
      "https://github.com/yt-dlp/yt-dlp/releases/latest/download/yt-dlp.exe",
      "yt-dlp.exe");
#else
  DownloadFileFromHTTPS(
      "https://github.com/yt-dlp/yt-dlp/releases/latest/download/yt-dlp",
      "yt-dlp");
  ExecuteCommand(boost::process::environment::find_executable("chmod").c_str(),
                 {"+x", "yt-dlp"});
#endif
  return 0;
}

int CheckUpdate() {
  char buf[16384];

  if (ValidateCommand("yt-dlp") != 0) {
    InstallYtdlp();
    return 0;
  }

  auto ytdlp_pipe = utils::OpenPipe("yt-dlp", {"-U"});

  while (true) {
    boost::system::error_code read_ec;
    size_t bytes_read =
        boost::asio::read(ytdlp_pipe, boost::asio::buffer(buf, 16384), read_ec);

    if (bytes_read > 0) {
      BOOST_LOG_TRIVIAL(info) << buf;
    }

    if (read_ec == boost::asio::error::eof || read_ec) {
      break;
    }
  }

  return 0;
}
}  // namespace utils