/**
 * @file src/streamlink_callbacks.h
 * @brief STREAMLINK-MOD-03 — session lifecycle callbacks emitted to the local
 *        STREAMLINK agent. Fire-and-forget HTTP POSTs to 127.0.0.1:9876.
 *
 * Why fire-and-forget? Apollo's streaming threads must never block on agent
 * availability. If the agent is offline, the callback silently drops the
 * notification. The agent re-syncs state via the polling GET /api/v1/streamlink/sessions
 * endpoint added in MOD-02.
 */
#pragma once

#include <string>

namespace streamlink {
  /**
   * @brief Notify the agent that an RTSP launch session was accepted.
   * @param session_uuid The session UUID (matches MOD-02 listing).
   * @param app_name The application about to be launched.
   */
  void notify_session_started(const std::string &session_uuid, const std::string &app_name);

  /**
   * @brief Notify the agent that the launch session was terminated /
   *        timed out / refused.
   * @param session_uuid The session UUID.
   * @param reason A short reason code: "timeout" | "cleared" | "client_disconnect" | "internal".
   */
  void notify_session_ended(const std::string &session_uuid, const std::string &reason);
}  // namespace streamlink
