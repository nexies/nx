//
// thread — represents a thread of execution with an attached nx::asio::io_context.
//
// Each object belongs to exactly one thread. Signals from the same thread are
// delivered directly (Direct); cross-thread signals are posted to the target
// thread's io_context (Queued).
//
// Hierarchy:
//   thread       — general thread (wraps std::thread + io_context)
//   local_thread — represents the calling OS thread (no std::thread is created)
//

#pragma once

#include <nx/common/helpers.hpp>
#include <nx/common/types/result.hpp>

#include <nx/asio/io_context.hpp>

#include <nx/core2/object/object.hpp>

#include <atomic>
#include <functional>
#include <memory>
#include <string>
#include <thread>

namespace nx::core {

class loop;

// ──────────────────────────────────────────────────────────────────────────────
// thread_id_t — lightweight identifier assigned sequentially at thread creation
// ──────────────────────────────────────────────────────────────────────────────

using thread_id_t        = std::uint32_t;
using native_thread_id_t = std::thread::id;

constexpr thread_id_t invalid_thread_id = 0;

// ──────────────────────────────────────────────────────────────────────────────
// thread
// ──────────────────────────────────────────────────────────────────────────────

class thread : public object {
public:
    using context_t = nx::asio::io_context;
    using task_t    = context_t::task_t;

    explicit thread(object * parent = nullptr);
    explicit thread(std::string name, object * parent = nullptr);
    ~thread() override;

    NX_DISABLE_COPY(thread)

    NX_OBJECT(thread)

    // ── Identification ────────────────────────────────────────────────────────

    NX_NODISCARD thread_id_t
    id() const noexcept;

    NX_NODISCARD native_thread_id_t
    native_id() const noexcept;

    NX_NODISCARD const std::string &
    name() const noexcept;

    // ── Lifecycle ─────────────────────────────────────────────────────────────

    // Start the thread (creates std::thread, runs io_context::run).
    // Returns error if already running.
    virtual nx::result<void>
    start();

    // Request the thread's event loop to stop gracefully.
    void
    quit();

    // Equivalent to quit() followed by a posted exit signal with an exit code.
    void
    exit(int code = 0);

    // Block the calling thread until this thread finishes.
    // Returns false on timeout or if the thread was never started.
    bool
    wait();

    NX_NODISCARD bool
    is_running() const noexcept;

    // ── Task dispatch ─────────────────────────────────────────────────────────

    // Schedule a task to run on this thread's event loop (thread-safe).
    template<typename Task>
    void post(Task && task)
    {
        nx::asio::post(context_, std::forward<Task>(task));
    }

    // Run the task immediately if already on this thread; post otherwise.
    template<typename Task>
    void dispatch(Task && task)
    {
        nx::asio::dispatch(context_, std::forward<Task>(task));
    }

    // ── Context access ────────────────────────────────────────────────────────

    NX_NODISCARD context_t &
    context() noexcept;

    NX_NODISCARD const context_t &
    context() const noexcept;

    // Current loop running on this thread (null when idle).
    NX_NODISCARD loop *
    current_loop() const noexcept;

    // ── Signals ───────────────────────────────────────────────────────────────

    NX_SIGNAL(started)
    NX_SIGNAL(finished)
    NX_SIGNAL(finished_with_code, int)

    // ── Static helpers ────────────────────────────────────────────────────────

    // Thread that the calling OS thread is associated with (null if unregistered).
    NX_NODISCARD static thread *
    current() noexcept;

    NX_NODISCARD static thread_id_t
    current_id() noexcept;

    // io_context of the calling thread.
    NX_NODISCARD static context_t &
    current_context();

protected:
    // Called inside the new std::thread before the event loop runs.
    virtual void
    _on_started();

    // Called inside the thread just before it exits.
    virtual void
    _on_finished(int exit_code);

    // Installs / removes this thread as the current thread for the calling OS thread.
    void
    _register_as_current();

    void
    _unregister_as_current();

    void
    _set_current_loop(loop * l) noexcept;

private:
    friend class loop;
    friend class local_thread;

    thread_id_t              id_ { invalid_thread_id };
    std::string              name_;
    context_t                context_;
    std::unique_ptr<std::thread> std_thread_;
    std::atomic_bool         running_ { false };
    loop *                   current_loop_ { nullptr };
    int                      exit_code_ { 0 };
};

// ──────────────────────────────────────────────────────────────────────────────
// local_thread — wraps the calling (already running) OS thread
//
// Typically used for the main thread:
//   auto main_thread = std::make_unique<nx::core::local_thread>();
//   // now Thread::current() returns this from the main thread
// ──────────────────────────────────────────────────────────────────────────────

class local_thread final : public thread {
public:
    explicit local_thread(object * parent = nullptr);
    explicit local_thread(std::string name, object * parent = nullptr);
    ~local_thread() override;

    NX_OBJECT(local_thread)

    // local_thread cannot be started — it is already running.
    nx::result<void>
    start() override;
};

// ──────────────────────────────────────────────────────────────────────────────
// thread_registry — global registry mapping OS thread IDs to thread objects
// ──────────────────────────────────────────────────────────────────────────────

namespace detail {

class thread_registry {
public:
    static thread_registry &
    instance();

    thread_id_t
    register_thread(thread * t);

    void
    unregister_thread(thread * t);

    NX_NODISCARD thread *
    find_by_id(thread_id_t id) const;

    NX_NODISCARD thread *
    find_by_native_id(native_thread_id_t native_id) const;

    void
    exit_all(int code = 0);

    void
    wait_all();

    NX_NODISCARD std::size_t
    count() const;

private:
    thread_registry() = default;

    mutable std::mutex                                    mutex_;
    std::unordered_map<thread_id_t, thread *>             by_id_;
    std::unordered_map<size_t, thread *>                  by_native_;
    thread_id_t                                           next_id_ { 1 };
};

} // namespace detail
} // namespace nx::core
