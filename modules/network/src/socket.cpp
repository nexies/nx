#include <nx/network/socket.hpp>

#include "detail/socket_impl.hpp"

namespace nx::network {

socket::socket(socket_type type, nx::core::object * parent)
    : io_device(parent)
    , type_(type)
{}

socket::~socket()
{
    close();
}

// ── State ─────────────────────────────────────────────────────────────────────

socket::state socket::socket_state() const noexcept { return state_; }
bool          socket::is_open()       const noexcept { return impl_ && impl_->is_open(); }

void socket::_set_state(state s)
{
    if (state_ == s) return;
    state_ = s;
    NX_EMIT(state_changed, s);
}

// ── Lifecycle ─────────────────────────────────────────────────────────────────

nx::result<void> socket::open(socket_family family)
{
    if (is_open())
        return nx::err::invalid_state("socket already open");

    auto * t = get_thread();
    if (!t)
        return nx::err::invalid_state("socket not assigned to a thread");

    impl_ = detail::socket_impl::make();

    auto r = impl_->create(family, type_);
    if (!r) {
        impl_.reset();
        return r;
    }

    impl_->attach(t->context(), [this](nx::asio::io_event ev) {
        _on_io_event(ev);
    });

    _set_state(state::open);
    return {};
}

void socket::close()
{
    if (!impl_) return;

    impl_->detach();
    impl_->destroy();
    impl_.reset();

    _set_state(state::closed);
}

// ── Sync wait ─────────────────────────────────────────────────────────────────

nx::result<void> socket::wait_for_read(std::chrono::milliseconds timeout)
{
    if (!impl_ || !impl_->is_open())
        return nx::err::invalid_state("socket not open");
    return impl_->wait_readable(timeout);
}

// ── Protected helpers ─────────────────────────────────────────────────────────

detail::socket_impl & socket::_impl() noexcept
{
    return *impl_;
}

const detail::socket_impl & socket::_impl() const noexcept
{
    return *impl_;
}

void socket::_adopt_impl(std::unique_ptr<detail::socket_impl> impl) noexcept
{
    if (impl_) {
        impl_->detach();
        impl_->destroy();
    }
    impl_ = std::move(impl);
}

// ── Option bridge ────────────────────────────────────────────────────────────

nx::result<void> socket::_set_option_raw(opt_level level, opt_name name,
                                          const void * val, std::size_t len)
{
    if (!impl_ || !impl_->is_open())
        return nx::err::invalid_state("socket not open");
    return impl_->set_option_raw(level, name, val, len);
}

nx::result<void> socket::_get_option_raw(opt_level level, opt_name name,
                                          void * val, std::size_t & len) const
{
    if (!impl_ || !impl_->is_open())
        return nx::err::invalid_state("socket not open");
    return impl_->get_option_raw(level, name, val, len);
}

// ── Thread change ─────────────────────────────────────────────────────────────

void socket::_on_thread_changed(nx::core::thread * /*old_t*/,
                                  nx::core::thread * new_t)
{
    if (!impl_ || !impl_->is_open() || !new_t) return;

    impl_->detach();
    impl_->attach(new_t->context(), [this](nx::asio::io_event ev) {
        _on_io_event(ev);
    });
    // Subclass is responsible for re-arming (arm_read / arm_write).
}

} // namespace nx::network
