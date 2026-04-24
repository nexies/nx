//
// timer — implementation of nx::core::timer
//

#include <nx/core2/timer/timer.hpp>
#include <nx/core2/thread/thread.hpp>

namespace nx::core {

timer::timer(object * parent)
    : object(parent)
{}

timer::timer(duration_t interval, timer_type type, object * parent)
    : object(parent)
{
    set_interval(interval);
    set_type(type);
}

timer::~timer()
{
    stop();
}

void
timer::start()
{
    auto * t = get_thread();
    if (!t)
        return;

    if (!asio_timer_)
        asio_timer_ = std::make_unique<nx::asio::steady_timer>(t->context());

    active_.store(true, std::memory_order_release);
    _arm();
}

void
timer::start(duration_t new_interval)
{
    set_interval(new_interval);
    start();
}

void
timer::stop()
{
    active_.store(false, std::memory_order_release);
    if (asio_timer_)
        asio_timer_->cancel();
}

bool
timer::is_active() const noexcept
{
    return active_.load(std::memory_order_acquire);
}

void
timer::_arm()
{
    if (!asio_timer_ || !active_.load(std::memory_order_acquire))
        return;

    asio_timer_->async_wait(get_interval(), [this]()
    {
        if (!active_.load(std::memory_order_acquire))
            return;

        _on_timeout();
    });
}

void
timer::_on_timeout()
{
    NX_EMIT(timeout);

    if (get_type() == timer_type::periodic && active_.load(std::memory_order_acquire))
        _arm();
    else
        active_.store(false, std::memory_order_release);
}

void
timer::_invoke_callback()
{
    if (callback_)
        callback_();
}

timer *
timer::single_shot(duration_t interval, object * parent, std::function<void()> callback)
{
    auto * t = new timer(interval, timer_type::single_shot, parent);
    t->callback_ = std::move(callback);
    connect(t, &timer::timeout, t, &timer::_invoke_callback);
    t->start();
    return t;
}

void
timer::_on_thread_changed(thread * /*old*/, thread * new_thread)
{
    if (!new_thread)
        return;

    // Re-create asio timer on the new context if currently armed
    const bool was_active = active_.load(std::memory_order_acquire);
    if (was_active && asio_timer_) {
        asio_timer_->cancel();
        asio_timer_ = std::make_unique<nx::asio::steady_timer>(new_thread->context());
        _arm();
    }
}

} // namespace nx::core
