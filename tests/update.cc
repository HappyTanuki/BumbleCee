#include "precomp.h"
#include "utils/update_checker.h"

int main() {
  boost::asio::io_context ctx;
  utils::CheckUpdate(ctx);
  return 0;
}