#include "utils/update_checker.h"

#include "precomp.h"
#include "utils/console.h"
#include "utils/file_downloader.h"

namespace utils {

int InstallYtdlp(boost::asio::io_context& ctx) {
  BOOST_LOG_TRIVIAL(warning) << "ytdlp is unavailable. downloading ytdlp...";
#ifdef WIN32
  DownloadFileFromHTTPS(
      "https://github.com/yt-dlp/yt-dlp/releases/latest/download/yt-dlp.exe",
      "yt-dlp.exe");
#else
  DownloadFileFromHTTPS(
      "https://github.com/yt-dlp/yt-dlp/releases/latest/download/yt-dlp",
      "yt-dlp");
  ExecuteCommand(ctx,
                 boost::process::environment::find_executable("chmod").string(),
                 {"+x", "yt-dlp"});
#endif
  return 0;
}

static int FindNewLinePos(std::string& string, int start_pos) {
  int newline_pos = start_pos;
  while (newline_pos < string.size()) {
    if (string[newline_pos] == '\n') {
      return newline_pos;
    }
    newline_pos++;
  }
  return newline_pos;
}

int CheckUpdate(boost::asio::io_context& ctx) {
  std::string output = "";
  int old_newline_pos = 0;
  int newline_pos = 0;

#ifdef WIN32
  if (ExecuteCommand(ctx, "yt-dlp.exe", {"--version", "--newline"}, output) !=
      0) {
    InstallYtdlp(ctx);
    ExecuteCommand(ctx, "yt-dlp.exe", {"--version", "--newline"}, output);
  }
  BOOST_LOG_TRIVIAL(info) << "yt-dlp version: "
                          << output.substr(0, output.size() - 1);
  output = "";
  ExecuteCommand(ctx, "yt-dlp.exe", {"-U", "--newline"}, output);
#else
  if (ExecuteCommand(ctx, "yt-dlp", {"--version", "--newline"}, output) != 0) {
    InstallYtdlp(ctx);
    ExecuteCommand(ctx, "yt-dlp", {"--version", "--newline"}, output);
  }
  BOOST_LOG_TRIVIAL(info) << "yt-dlp version: "
                          << output.substr(0, output.size() - 1);
  output = "";
  ExecuteCommand(ctx, "yt-dlp", {"-U", "--newline"}, output);
#endif
  while (newline_pos < output.size()) {
    old_newline_pos = newline_pos;
    newline_pos = FindNewLinePos(output, newline_pos);
    BOOST_LOG_TRIVIAL(info) << output.substr(old_newline_pos, newline_pos - 1);
    newline_pos++;
  }

  return 0;
}
}  // namespace utils