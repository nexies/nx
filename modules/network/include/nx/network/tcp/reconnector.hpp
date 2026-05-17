#pragma once

#include <nx/network/tcp/socket.hpp>

#include <nx/common/helpers.hpp>
#include <nx/core2/object/object.hpp>
#include <nx/core2/timer/timer.hpp>

#include <chrono>

namespace nx::network::tcp {

// ── tcp::reconnector ──────────────────────────────────────────────────────────
//
// Wraps a tcp::socket and automatically reconnects after a disconnect.
//
// connect(ep) — start connecting; retries on disconnect/error with exponential
//   backoff starting at 1 s, doubling up to 30 s.
// disconnect() — stop reconnecting and close the socket (no further retries).
//
// The data path is untouched: read_accumulator / framers connect to socket
// signals directly and work regardless of reconnects.

class reconnector : public nx::core::object {
public:
    NX_OBJECT(reconnector)
    NX_DISABLE_COPY(reconnector)
    NX_DISABLE_MOVE(reconnector)

    explicit reconnector(tcp::socket * socket,
                         nx::core::object * parent = nullptr);

    // ── Control ───────────────────────────────────────────────────────────────

    void connect   (const endpoint & ep);
    void disconnect();

    // ── Signals ───────────────────────────────────────────────────────────────

    NX_SIGNAL(connected)
    NX_SIGNAL(disconnected)
    NX_SIGNAL(error_occurred, nx::error)

private:
    void _try_connect();
    void _schedule_retry();

    tcp::socket *    socket_;
    endpoint         ep_;
    nx::core::timer  timer_ { this };
    int              attempt_ { 0 };
    bool             active_  { false };

    static constexpr int max_delay_s = 30;
};

} // namespace nx::network::tcp
