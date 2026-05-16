#pragma once

#include <nx/network/socket.hpp>

namespace nx::network::udp {

// ── udp::socket ───────────────────────────────────────────────────────────────
//
// Datagram (UDP) socket.
//
// Async receive:
//   Connect ready_read signal and call recv_from() inside the slot.
//
// Async send:
//   Call send_to() directly; UDP never blocks for connection setup.

class socket final : public nx::network::socket {
public:
    NX_OBJECT(socket)
    NX_DISABLE_COPY(socket)
    NX_DISABLE_MOVE(socket)

    explicit socket(nx::core::object * parent = nullptr);

    // ── Socket configuration ──────────────────────────────────────────────────

    nx::result<void> bind   (const endpoint & local_ep);
    nx::result<void> connect(const endpoint & remote_ep); // set default destination

    // ── I/O ───────────────────────────────────────────────────────────────────

    // io_device interface — uses default destination (set via connect()).
    nx::result<std::size_t> read (char * buf, std::size_t max_len) override;
    nx::result<std::size_t> write(const char * buf, std::size_t len) override;

    // Datagram-oriented — always preferred over read()/write() for UDP.
    nx::result<std::size_t> recv_from(char * buf, std::size_t max_len, endpoint & from);
    nx::result<std::size_t> send_to  (const char * buf, std::size_t len, const endpoint & to);

protected:
    void _on_io_event(nx::asio::io_event ev) override;

private:
    endpoint dest_; // default destination (set by connect())
};

} // namespace nx::network::udp
