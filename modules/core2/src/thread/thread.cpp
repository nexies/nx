//
// thread — implementation of nx::core::thread and nx::core::local_thread
//

#include <nx/core2/thread/thread.hpp>
#include <nx/core2/loop/loop.hpp>

#include <nx/core2/detail/logger_defs.hpp>

#include <cassert>
#include <stdexcept>

namespace nx::core {

// ──────────────────────────────────────────────────────────────────────────────
// thread_registry
// ──────────────────────────────────────────────────────────────────────────────

namespace detail {

thread_registry &
thread_registry::instance()
{
    static thread_registry inst;
    return inst;
}

thread_id_t
thread_registry::register_thread(thread * t)
{
    std::lock_guard lock { mutex_ };
    const thread_id_t id = next_id_++;
    by_id_[id] = t;
    const std::size_t native_hash =
        std::hash<native_thread_id_t>{}(t->native_id());
    by_native_[native_hash] = t;
    return id;
}

void
thread_registry::unregister_thread(thread * t)
{
    std::lock_guard lock { mutex_ };
    by_id_.erase(t->id());
    const std::size_t native_hash =
        std::hash<native_thread_id_t>{}(t->native_id());
    by_native_.erase(native_hash);
}

thread *
thread_registry::find_by_id(thread_id_t id) const
{
    std::lock_guard lock { mutex_ };
    auto it = by_id_.find(id);
    return it != by_id_.end() ? it->second : nullptr;
}

thread *
thread_registry::find_by_native_id(native_thread_id_t native_id) const
{
    std::lock_guard lock { mutex_ };
    const std::size_t h = std::hash<native_thread_id_t>{}(native_id);
    auto it = by_native_.find(h);
    return it != by_native_.end() ? it->second : nullptr;
}

void
thread_registry::exit_all(int code)
{
    std::lock_guard lock { mutex_ };
    for (auto & [id, t] : by_id_)
        t->exit(code);
}

void
thread_registry::wait_all()
{
    // Snapshot to avoid holding lock during wait
    std::vector<thread *> ts;
    {
        std::lock_guard lock { mutex_ };
        for (auto & [id, t] : by_id_)
            ts.push_back(t);
    }
    for (auto * t : ts)
        t->wait();
}

std::size_t
thread_registry::count() const
{
    std::lock_guard lock { mutex_ };
    return by_id_.size();
}

} // namespace detail

// ──────────────────────────────────────────────────────────────────────────────
// Thread-local current thread pointer
// ──────────────────────────────────────────────────────────────────────────────

NX_THREAD_LOCAL thread * tl_current_thread = nullptr;

// ──────────────────────────────────────────────────────────────────────────────
// thread
// ──────────────────────────────────────────────────────────────────────────────

thread::thread(object * parent)
    : thread("", parent)
{}

thread::thread(std::string name, object * parent)
    : object(parent)
    , name_(std::move(name))
{
    // thread objects themselves live on the thread that created them,
    // but the io_context belongs to this new thread — set after start().
}

thread::~thread()
{
    if (is_running())
        exit(0);

    if (std_thread_ && std_thread_->joinable())
        std_thread_->join();

    detail::thread_registry::instance().unregister_thread(this);
}

thread_id_t
thread::id() const noexcept
{
    return id_;
}

native_thread_id_t
thread::native_id() const noexcept
{
    if (std_thread_)
        return std_thread_->get_id();
    return {};
}

const std::string &
thread::name() const noexcept
{
    return name_;
}

nx::result<void>
thread::start()
{
    if (running_.load(std::memory_order_acquire))
        return nx::error { std::make_error_code(std::errc::operation_in_progress), "thread already running" };

    running_.store(true, std::memory_order_release);
    context_.restart();

    std_thread_ = std::make_unique<std::thread>([this]()
    {
        _register_as_current();
        _on_started();
        NX_EMIT(started);

        try {
            context_.run();
        } catch (const std::exception & e) {
            nxError("thread '{}': unhandled exception: {}", name_, e.what());
        }

        running_.store(false, std::memory_order_release);
        _on_finished(exit_code_);
        NX_EMIT(finished_with_code, exit_code_);
        NX_EMIT(finished);
        _unregister_as_current();
    });

    return {};
}

void
thread::quit()
{
    exit(0);
}

void
thread::exit(int code)
{
    exit_code_ = code;
    context_.stop();
}

bool
thread::wait()
{
    if (std_thread_ && std_thread_->joinable()) {
        std_thread_->join();
        return true;
    }
    return false;
}

bool
thread::is_running() const noexcept
{
    return running_.load(std::memory_order_acquire);
}

thread::context_t &
thread::context() noexcept
{
    return context_;
}

const thread::context_t &
thread::context() const noexcept
{
    return context_;
}

loop *
thread::current_loop() const noexcept
{
    return current_loop_;
}

thread *
thread::current() noexcept
{
    return tl_current_thread;
}

thread_id_t
thread::current_id() noexcept
{
    auto * t = tl_current_thread;
    return t ? t->id() : invalid_thread_id;
}

thread::context_t &
thread::current_context()
{
    auto * t = tl_current_thread;
    if (!t)
        throw std::runtime_error("thread::current_context(): no thread on this OS thread");
    return t->context();
}

void
thread::_on_started()
{}

void
thread::_on_finished(int /*exit_code*/)
{}

void
thread::_register_as_current()
{
    tl_current_thread = this;
    id_ = detail::thread_registry::instance().register_thread(this);
}

void
thread::_unregister_as_current()
{
    detail::thread_registry::instance().unregister_thread(this);
    tl_current_thread = nullptr;
}

void
thread::_set_current_loop(loop * l) noexcept
{
    current_loop_ = l;
}

// ──────────────────────────────────────────────────────────────────────────────
// local_thread
// ──────────────────────────────────────────────────────────────────────────────

local_thread::local_thread(object * parent)
    : local_thread("main", parent)
{}

local_thread::local_thread(std::string name, object * parent)
    : thread(std::move(name), parent)
{
    // Register the calling OS thread as the current thread immediately
    _register_as_current();
    running_.store(true, std::memory_order_release);
}

local_thread::~local_thread()
{
    _unregister_as_current();
    running_.store(false, std::memory_order_release);
}

nx::result<void>
local_thread::start()
{
    return nx::error { std::make_error_code(std::errc::operation_not_supported),
                       "local_thread cannot be started — it represents the calling thread" };
}

} // namespace nx::core
