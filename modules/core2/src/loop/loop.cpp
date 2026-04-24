//
// loop — implementation of nx::core::loop
//

#include <nx/core2/loop/loop.hpp>
#include <nx/core2/thread/thread.hpp>

namespace nx::core {

// Thread-local pointer to the innermost running loop
NX_THREAD_LOCAL loop * tl_current_loop = nullptr;

// ──────────────────────────────────────────────────────────────────────────────
// loop
// ──────────────────────────────────────────────────────────────────────────────

loop::loop(object * parent)
    : object(parent)
{}

loop::~loop()
{
    if (is_running())
        exit(0);
}

int
loop::exec()
{
    auto * t = get_thread();
    if (!t)
        return -1;

    _install_on_thread(t);

    stop_requested_.store(false, std::memory_order_release);
    running_.store(true, std::memory_order_release);

    // Run until stop() is requested from inside the context
    t->context().run();

    running_.store(false, std::memory_order_release);
    NX_EMIT(about_to_quit);

    _uninstall_from_thread(t);
    return exit_code_.load(std::memory_order_acquire);
}

std::size_t
loop::process_events()
{
    auto * t = get_thread();
    if (!t)
        return 0;
    return t->context().poll();
}

std::size_t
loop::process_events_for(nx::asio::duration duration)
{
    auto * t = get_thread();
    if (!t)
        return 0;
    return t->context().run_for(duration);
}

void
loop::quit()
{
    exit(0);
}

void
loop::exit(int code)
{
    exit_code_.store(code, std::memory_order_release);
    stop_requested_.store(true, std::memory_order_release);

    if (auto * t = get_thread())
        t->context().stop();
}

void
loop::wakeup()
{
    if (auto * t = get_thread())
        nx::asio::post(t->context(), [](){});
}

bool
loop::is_running() const noexcept
{
    return running_.load(std::memory_order_acquire);
}

int
loop::exit_code() const noexcept
{
    return exit_code_.load(std::memory_order_acquire);
}

loop *
loop::current() noexcept
{
    return tl_current_loop;
}

void
loop::_install_on_thread(thread * t)
{
    tl_current_loop = this;
    t->_set_current_loop(this);
}

void
loop::_uninstall_from_thread(thread * t)
{
    t->_set_current_loop(nullptr);
    tl_current_loop = nullptr;
}

} // namespace nx::core
