#include "utils/console.h"

#include "precomp.h"

namespace utils {

int ValidateCommand(boost::asio::io_context& ctx, std::string cmd,
                    const std::vector<std::string>& args) {
  try {
    ExecuteCommand(ctx, cmd, args);
  } catch (const boost::process::system_error& e) {
    return -1;
  }
  return 0;
}

int ExecuteCommand(boost::asio::io_context& ctx, const std::string& cmd,
                   const std::vector<std::string>& args) {
  std::string ignored;
  return ExecuteCommand(ctx, cmd, args, ignored);
}

int ExecuteCommand(boost::asio::io_context& ctx, const std::string& cmd,
                   std::string& result) {
  std::vector<std::string> ignored;
  return ExecuteCommand(ctx, cmd, ignored, result);
}

int ExecuteCommand(boost::asio::io_context& ctx, const std::string& cmd,
                   const std::vector<std::string>& args, std::string& result) {
  try {
    boost::system::error_code ec;
    boost::process::popen proc(
        ctx, cmd, args, boost::process::process_stdio{{}, nullptr, nullptr});

    boost::asio::read(proc, boost::asio::dynamic_buffer(result), ec);

    proc.wait();

    return proc.exit_code();
  } catch (const boost::process::system_error& e) {
    return -1;
  }
}

boost::process::popen OpenPipe(boost::asio::io_context& ctx,
                               const std::string& cmd,
                               const std::vector<std::string>& args) {
  return boost::process::popen(
      ctx, cmd, args, boost::process::process_stdio{nullptr, nullptr, nullptr});
};

}  // namespace utils