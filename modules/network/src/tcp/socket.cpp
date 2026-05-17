#include <nx/network/tcp/socket.hpp>

#include "detail/socket_impl.hpp"

namespace nx::network::tcp {

using io_event = nx::asio::io_event;

socket::socket(nx::core::object * parent)
    : nx::network::socket(socket_type::tcp, parent)
{}

// ── Connect ───────────────────────────────────────────────────────────────────

nx::result<void> socket::connect(const endpoint & remote_ep)
{
    if (!is_open())
        return nx::err::invalid_state("socket not open");

    auto r = _impl().connect(remote_ep);
    if (!r) return r;

    _set_state(state::connecting);

    // Wait for write-ready event — signals connect completion on both platforms.
    _impl().arm_write();
    return {};
}

nx::result<void> socket::connect_sync(const endpoint & remote_ep,
                                        std::chrono::milliseconds timeout)
{
    if (!is_open())
        return nx::err::invalid_state("socket not open");

    auto r = _impl().connect(remote_ep);
    if (!r) return r;

    _set_state(state::connecting);

    // Wait until connect completes (write-readiness signals completion on all platforms).
    r = _impl().wait_writable(timeout);
    if (!r) return r;

    // Check whether connect actually succeeded.
    const int so_err = _impl().get_so_error();
    if (so_err != 0)
        return nx::err::runtime_error(so_err, "connect failed");

    _set_state(state::connected);
    _impl().arm_read();
    return {};
}

void socket::disconnect()
{
    close();
    NX_EMIT(disconnected);
}

// ── I/O ───────────────────────────────────────────────────────────────────────

nx::result<std::size_t> socket::read(char * buf, std::size_t max_len)
{
    if (!is_open())
        return nx::err::invalid_state("socket not open");
    return _impl().read(buf, max_len);
}

nx::result<std::size_t> socket::write(const char * buf, std::size_t len)
{
    if (!is_open())
        return nx::err::invalid_state("socket not open");
    return _impl().write(buf, len);
}

// ── Accept (called by tcp::server) ────────────────────────────────────────────

void socket::_accept_from(std::unique_ptr<detail::socket_impl> impl)
{
    _adopt_impl(std::move(impl));

    auto * t = get_thread();
    if (t) {
        _impl().attach(t->context(), [this](nx::asio::io_event ev) {
            _on_io_event(ev);
        });
        _impl().arm_read();
    }
    _set_state(state::connected);
}

// ── Event handling ────────────────────────────────────────────────────────────

void socket::_on_io_event(io_event ev)
{
    if (socket_state() == state::connecting) {
        if ((ev & io_event::error) != io_event::none) {
            _set_state(state::open);
            NX_EMIT(error_occurred, nx::err::runtime_error("connect failed"));
            return;
        }
        if ((ev & io_event::write) != io_event::none) {
            _impl().disarm_write();
            _set_state(state::connected);
            _impl().arm_read();
            NX_EMIT(connected);
            return;
        }
    }

    if ((ev & io_event::read) != io_event::none) {
        char buf[4096];
        while (true) {
            auto r = _impl().read(buf, sizeof(buf));
            if (r.is_error()) {
                if (!detail::socket_impl::is_would_block(r.error()))
                    NX_EMIT(error_occurred, r.error());
                break;
            }
            if (r.value() == 0) {
                _set_state(state::closed);
                NX_EMIT(disconnected);
                return;
            }
            NX_EMIT(data_received, nx::span<const char>(buf, r.value()));
        }
        _impl().arm_read();
    }
    if ((ev & io_event::write) != io_event::none) {
        _impl().disarm_write();
        NX_EMIT(bytes_written, std::uint64_t { 0 });
    }
    if ((ev & io_event::hangup) != io_event::none) {
        _set_state(state::closed);
        NX_EMIT(disconnected);
    }
    if ((ev & io_event::error) != io_event::none) {
        NX_EMIT(error_occurred, nx::err::runtime_error("tcp socket error"));
    }
}

} // namespace nx::network::tcp
