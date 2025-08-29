#include "utils/console.h"

#include "precomp.h"

namespace utils {

int ValidateCommand(std::string cmd) {
  try {
    ExecuteCommand(cmd);
  } catch (const boost::process::system_error& e) {
    return -1;
  }
  return 0;
}

int ExecuteCommand(const std::string& cmd,
                   const std::vector<std::string>& args) {
  std::string ignored;
  return ExecuteCommand(cmd, args, ignored);
}

int ExecuteCommand(const std::string& cmd, std::string& result) {
  std::vector<std::string> ignored;
  return ExecuteCommand(cmd, ignored, result);
}

int ExecuteCommand(const std::string& cmd, const std::vector<std::string>& args,
                   std::string& result) {
  boost::system::error_code ec;
  boost::asio::io_context ctx;
  boost::process::popen proc(
      ctx, cmd, args, boost::process::process_stdio{{}, nullptr, nullptr});

  boost::asio::read(proc, boost::asio::dynamic_buffer(result), ec);

  if (ec != boost::asio::error::eof) {
    return -1;
  }

  proc.wait();

  return proc.exit_code();
}

boost::process::popen OpenPipe(const std::string& cmd,
                               const std::vector<std::string>& args) {
  boost::asio::io_context ctx;
  boost::process::popen proc(
      ctx, cmd, args, boost::process::process_stdio{{}, nullptr, nullptr});
  return proc;
};

}  // namespace utils