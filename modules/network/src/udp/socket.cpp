#include <nx/network/udp/socket.hpp>

#include "detail/socket_impl.hpp"

namespace nx::network::udp {

using io_event = nx::asio::io_event;

socket::socket(nx::core::object * parent)
    : nx::network::socket(socket_type::udp, parent)
{}

// ── Configuration ─────────────────────────────────────────────────────────────

nx::result<void> socket::bind(const endpoint & local_ep)
{
    if (!is_open())
        return nx::err::invalid_state("socket not open");

    auto r = _impl().bind(local_ep);
    if (!r) return r;

    _set_state(state::bound);
    _impl().arm_read(); // start watching for incoming datagrams
    return {};
}

nx::result<void> socket::connect(const endpoint & remote_ep)
{
    if (!is_open())
        return nx::err::invalid_state("socket not open");

    dest_ = remote_ep;

    // UDP "connect" just sets the kernel-level default destination / filter.
    auto r = _impl().connect(remote_ep);
    if (!r) return r;

    _set_state(state::connected);
    _impl().arm_read();
    return {};
}

// ── I/O ───────────────────────────────────────────────────────────────────────

nx::result<std::size_t> socket::read(char * buf, std::size_t max_len)
{
    if (!is_open())
        return nx::err::invalid_state("socket not open");

    endpoint ignored;
    return _impl().read_from(buf, max_len, ignored);
}

nx::result<std::size_t> socket::write(const char * buf, std::size_t len)
{
    if (!is_open())
        return nx::err::invalid_state("socket not open");

    if (dest_.port == 0)
        return nx::err::invalid_state("no default destination — use send_to()");

    return _impl().write_to(buf, len, dest_);
}

nx::result<std::size_t> socket::recv_from(char * buf, std::size_t max_len, endpoint & from)
{
    if (!is_open())
        return nx::err::invalid_state("socket not open");

    return _impl().read_from(buf, max_len, from);
}

nx::result<std::size_t> socket::send_to(const char * buf, std::size_t len,
                                          const endpoint & to)
{
    if (!is_open())
        return nx::err::invalid_state("socket not open");

    return _impl().write_to(buf, len, to);
}

// ── Event handling ────────────────────────────────────────────────────────────

void socket::_on_io_event(io_event ev)
{
    if ((ev & io_event::read) != io_event::none) {
        NX_EMIT(ready_read);
        _impl().arm_read(); // re-arm for next datagram
    }
    if ((ev & (io_event::error | io_event::hangup)) != io_event::none) {
        NX_EMIT(error_occurred, nx::err::runtime_error("udp socket error"));
    }
}

} // namespace nx::network::udp
