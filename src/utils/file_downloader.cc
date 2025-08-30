#include "utils/file_downloader.h"

#include <fstream>

#include "boost/asio/ssl.hpp"
#include "boost/beast.hpp"
#include "precomp.h"

namespace utils {

void DownloadFileFromHTTPS(std::string url, std::string filename) {
  int max_redirects = 5;

  std::string current_host;
  std::string current_path;

  if (url.find("https://") == 0) {
    url.erase(0, 8);  // https:// 제거
    auto pos = url.find('/');
    if (pos != std::string::npos) {
      current_host = url.substr(0, pos);
      current_path = url.substr(pos);
    } else {
      current_host = url;
      current_path = "/";
    }
  } else {
    // 상대 경로일 경우
    current_path = url;
  }

  for (int redirect = 0; redirect < max_redirects; ++redirect) {
    boost::asio::io_context ioc;
    boost::asio::ssl::context ctx(boost::asio::ssl::context::tlsv12_client);
    ctx.set_default_verify_paths();

    boost::beast::ssl_stream<boost::beast::tcp_stream> stream(ioc, ctx);

    // DNS 해석
    boost::asio::ip::tcp::resolver resolver(ioc);
    auto const results = resolver.resolve(current_host, "443");

    // TCP 연결
    boost::beast::get_lowest_layer(stream).connect(results);

    // SSL 핸드셰이크
    stream.handshake(boost::asio::ssl::stream_base::client);

    // HTTP GET 요청
    boost::beast::http::request<boost::beast::http::string_body> req{
        boost::beast::http::verb::get, current_path, 11};
    req.set(boost::beast::http::field::host, current_host);
    req.set(boost::beast::http::field::user_agent, "Mozilla/5.0");

    boost::beast::http::write(stream, req);

    boost::beast::flat_buffer buffer;

    // 응답 파서 만들기
    boost::beast::http::response_parser<boost::beast::http::dynamic_body>
        parser;

    // body 제한 해제 (무제한)
    parser.body_limit((std::numeric_limits<std::uint64_t>::max)());

    // 응답 읽기
    boost::beast::http::read(stream, buffer, parser);

    auto res = parser.release();

    int status = res.result_int();
    if (status / 100 == 3) {
      // 리다이렉트 처리
      auto loc = res[boost::beast::http::field::location];
      if (loc.empty()) {
        throw std::runtime_error("Redirect without Location header");
      }

      // 새 호스트/경로 파싱
      std::string new_url(loc);
      if (new_url.find("https://") == 0) {
        new_url.erase(0, 8);  // https:// 제거
        auto pos = new_url.find('/');
        if (pos != std::string::npos) {
          current_host = new_url.substr(0, pos);
          current_path = new_url.substr(pos);
        } else {
          current_host = new_url;
          current_path = "/";
        }
      } else {
        // 상대 경로일 경우
        current_path = new_url;
      }

      continue;
    }

    if (status != 200) {
      throw std::runtime_error("HTTP request failed with status " +
                               std::to_string(status));
    }

    // 파일 저장
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
      throw std::runtime_error("Could not open file for writing: " + filename);
    }
    for (auto const& buffer_part : res.body().data()) {
      file.write(reinterpret_cast<const char*>(buffer_part.data()),
                 buffer_part.size());
    }

    return;
  }

  throw std::runtime_error("Too many redirects");
}
}  // namespace utils