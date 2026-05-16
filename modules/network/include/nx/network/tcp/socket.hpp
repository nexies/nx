#pragma once

#include <nx/network/socket.hpp>

namespace nx::network::tcp {

// ── tcp::socket ───────────────────────────────────────────────────────────────
//
// TCP stream socket (client side).
//
// Async connect:
//   Call connect(ep); listen to the connected signal; then start reading.
//
// Sync connect:
//   Call connect_sync(ep, timeout).

class socket final : public nx::network::socket {
public:
    NX_OBJECT(socket)
    NX_DISABLE_COPY(socket)
    NX_DISABLE_MOVE(socket)

    explicit socket(nx::core::object * parent = nullptr);

    // ── Connect / disconnect ──────────────────────────────────────────────────

    // Non-blocking connect.  Fires connected or error_occurred when done.
    nx::result<void> connect(const endpoint & remote_ep);

    // Blocking connect.  Returns when connected or on timeout / error.
    nx::result<void> connect_sync(const endpoint & remote_ep,
                                   std::chrono::milliseconds timeout);

    void disconnect();

    // ── I/O ───────────────────────────────────────────────────────────────────

    nx::result<std::size_t> read (char * buf, std::size_t max_len) override;
    nx::result<std::size_t> write(const char * buf, std::size_t len) override;

    // ── Signals ───────────────────────────────────────────────────────────────

    NX_SIGNAL(connected)
    NX_SIGNAL(disconnected)

protected:
    void _on_io_event(nx::asio::io_event ev) override;

private:
    friend class server; // server calls _accept_from()

    // Called by tcp::server when an incoming connection has been accepted.
    // impl is a pre-built socket_impl; this method attaches it and arms reads.
    void _accept_from(std::unique_ptr<detail::socket_impl> impl);
};

} // namespace nx::network::tcp
