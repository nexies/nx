#include <nx/network/tcp/server.hpp>
#include <nx/network/tcp/socket.hpp>

#include "detail/socket_impl.hpp"

namespace nx::network::tcp {

using io_event = nx::asio::io_event;

server::server(nx::core::object * parent)
    : nx::core::object(parent)
{}

server::~server()
{
    close();
}

// ── Control ───────────────────────────────────────────────────────────────────

nx::result<void> server::listen(const endpoint & local_ep,
                                  socket_family    family,
                                  int              backlog)
{
    if (impl_)
        return nx::err::invalid_state("server already listening");

    auto * t = get_thread();
    if (!t)
        return nx::err::invalid_state("server not assigned to a thread");

    family_ = family;
    impl_   = detail::socket_impl::make();

    auto r = impl_->create(family, socket_type::tcp);
    if (!r) { impl_.reset(); return r; }

    // Allow quick restart after close.
    {
        int v = 1;
        impl_->set_option_raw(opt_level::socket, opt_name::reuse_address, &v, sizeof(v));
    }

    r = impl_->bind(local_ep);
    if (!r) { impl_.reset(); return r; }

    r = impl_->listen(backlog);
    if (!r) { impl_.reset(); return r; }

    impl_->attach(t->context(), [this](io_event ev) { _on_io_event(ev); });
    _arm();

    return {};
}

void server::close()
{
    if (!impl_) return;
    impl_->detach();
    impl_->destroy();
    impl_.reset();
}

bool server::is_listening() const noexcept
{
    return impl_ && impl_->is_open();
}

// ── Thread change ─────────────────────────────────────────────────────────────

void server::_on_thread_changed(nx::core::thread * /*old_t*/,
                                  nx::core::thread * new_t)
{
    if (!impl_ || !impl_->is_open() || !new_t) return;

    impl_->detach();
    impl_->attach(new_t->context(), [this](io_event ev) { _on_io_event(ev); });
    _arm();
}

// ── Internal ──────────────────────────────────────────────────────────────────

void server::_arm()
{
    if (impl_) impl_->arm_read();
}

void server::_on_io_event(io_event ev)
{
    if ((ev & io_event::error) != io_event::none) {
        NX_EMIT(error_occurred, nx::err::runtime_error("server socket error"));
        return;
    }

    if ((ev & io_event::read) == io_event::none) return;

    // Accept all pending connections.
    while (true) {
        endpoint remote_ep;
        auto accepted = impl_->accept(remote_ep);

        if (accepted.is_error()) {
            if (!detail::socket_impl::is_would_block(accepted.error()))
                NX_EMIT(error_occurred, accepted.error());
            break;
        }

        // Create tcp::socket that adopts the accepted impl.
        // Parent = this server so lifetime is managed; caller can re-parent.
        auto conn = std::make_shared<tcp::socket>(this);
        conn->_accept_from(std::move(accepted.value()));
        NX_EMIT(new_connection, conn);
    }

    _arm(); // re-arm for next batch of connections
}

} // namespace nx::network::tcp
