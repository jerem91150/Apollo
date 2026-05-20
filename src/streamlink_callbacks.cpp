/**
 * @file src/streamlink_callbacks.cpp
 * @brief STREAMLINK-MOD-03 — implementation.
 *
 * Uses boost::asio synchronously inside a detached worker thread so we never
 * import any new third-party HTTP client. The POSTs target the agent's
 * `/api/v1/internal/session-event` endpoint with a tiny JSON body.
 */

#include "streamlink_callbacks.h"

#include <thread>
#include <string>
#include <string_view>

#include <boost/asio.hpp>
#include <nlohmann/json.hpp>

#include "logging.h"

using namespace std::literals;

namespace {
  constexpr const char *AGENT_HOST = "127.0.0.1";
  constexpr const char *AGENT_PORT = "9876";

  void fire_and_forget_post(const std::string &json_body) {
    std::thread([body = json_body]() {
      try {
        boost::asio::io_context io;
        boost::asio::ip::tcp::resolver resolver(io);
        auto endpoints = resolver.resolve(AGENT_HOST, AGENT_PORT);

        boost::asio::ip::tcp::socket sock(io);
        boost::asio::connect(sock, endpoints);

        std::string req =
          "POST /api/v1/internal/session-event HTTP/1.1\r\n"
          "Host: 127.0.0.1:9876\r\n"
          "Content-Type: application/json\r\n"
          "Content-Length: " + std::to_string(body.size()) + "\r\n"
          "Connection: close\r\n"
          "\r\n" + body;

        boost::asio::write(sock, boost::asio::buffer(req));

        // We don't care about the response. Read briefly so the server can
        // flush; then drop the connection.
        boost::system::error_code ec;
        std::array<char, 256> dump;
        (void) sock.read_some(boost::asio::buffer(dump), ec);

        boost::system::error_code ignore;
        sock.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignore);
      } catch (...) {
        // Agent is offline or rejected: silently drop. The agent will re-sync
        // via the polling /api/v1/streamlink/sessions endpoint.
      }
    }).detach();
  }
}  // namespace

namespace streamlink {

void notify_session_started(const std::string &session_uuid, const std::string &app_name) {
  nlohmann::json j;
  j["event"] = "session_started";
  j["session_uuid"] = session_uuid;
  j["app_name"] = app_name;
  fire_and_forget_post(j.dump());
}

void notify_session_ended(const std::string &session_uuid, const std::string &reason) {
  nlohmann::json j;
  j["event"] = "session_ended";
  j["session_uuid"] = session_uuid;
  j["reason"] = reason;
  fire_and_forget_post(j.dump());
}

}  // namespace streamlink
