//
// Created by nexie on 21.03.2026.
//

#include "context_impl.hpp"

// #include <bits/valarray_array.h>

// #include <ppltasks.h>

#include "nx/asio/backend/backend.hpp"
#include "../../include/nx/asio/context/reactor_handle.hpp"


namespace nx::asio
{
    io_context::impl::impl(std::unique_ptr<backend> backend)
        : backend_(std::move(backend))
          , stopped_(false)
          , running_(false)
    {
    }

    io_context::impl::~impl()
    {
        if (running_.load(std::memory_order_acquire))
            stop();
    }

    void io_context::impl::post(task_t task)
    {
        std::lock_guard lg(incoming_mutex_);
        ready_.push_back(std::move(task));
        backend_->wake();
    }

    void io_context::impl::dispatch(task_t task)
    {
        if (is_running_in_this_thread())
            task();
        else
            post(std::move(task));
    }

    std::size_t io_context::impl::run_once()
    {
        return 0;
    }

    std::size_t io_context::impl::run(std::optional<duration> max_duration)
    {
        running_.store(true, std::memory_order_release);

        {
            std::lock_guard lg{state_mutex_};
            owner_thread_id = std::this_thread::get_id();
        }

        std::size_t tasks_processed { 0 };
        const auto deadline = max_duration.has_value()
            ? std::optional<time_point>(clock::now() + *max_duration)
            : std::nullopt;

        while (!stopped_.load(std::memory_order_acquire))
        {
            drainExpiredTimers();
            tasks_processed += executeReady();

            // Recompute remaining time each iteration so backend_->wait never
            // sleeps longer than the time left in the run_for/run_until window.
            std::optional<duration> remaining;
            if (deadline.has_value()) {
                remaining = *deadline - clock::now();
                if (*remaining <= duration::zero())
                    break;
            }

            auto timeout = computeWaitTimeout(remaining);
            backend_event events[64];
            auto event_count = backend_->wait(events, 64, timeout);

            processBackendEvents(events, event_count);
        }
        running_.store(false);
        return tasks_processed;
    }

    std::size_t io_context::impl::poll()
    {
        running_.store(true, std::memory_order_release);

        {
            std::lock_guard lg{state_mutex_};
            owner_thread_id = std::this_thread::get_id();
        }

        std::size_t tasks_processed{0};
        drainExpiredTimers();
        tasks_processed += executeReady();

        backend_event events[64];
        auto event_count = backend_->wait(events, 64, duration::zero());
        processBackendEvents(events, event_count);
        running_.store(false);
        return tasks_processed;
    }

    std::size_t io_context::impl::poll_once()
    {
        running_.store(true, std::memory_order_release);

        {
            std::lock_guard lg{state_mutex_};
            owner_thread_id = std::this_thread::get_id();
        }

        std::size_t tasks_processed{0};
        drainExpiredTimers();
        tasks_processed += execute_one_ready();

        backend_event events[64];
        auto event_count = backend_->wait(events, 64, duration::zero());
        processBackendEvents(events, event_count);
        running_.store(false);
        return tasks_processed;
    }

    void io_context::impl::stop()
    {
        stopped_.store(true, std::memory_order_release);
        backend_->wake();
    }

    void io_context::impl::restart()
    {
        stopped_.store(false, std::memory_order_release);
        backend_->wake();
    }

    bool io_context::impl::stopped() const noexcept
    {
        return stopped_.load(std::memory_order_acquire);
    }

    bool io_context::impl::is_running_in_this_thread() const noexcept
    {
        if (!running_.load(std::memory_order_acquire))
            return false;
        {
            std::lock_guard lg{state_mutex_};
            return (std::this_thread::get_id() == owner_thread_id);
        }
    }

    timer_id io_context::impl::create_timer(time_point expiry, task_t && task)
    {
        auto id = ++next_timer_id_;
        auto timer_op = std::make_shared<TimerOp>(expiry, std::forward<task_t>(task), false, id);

        if (!timer_op)
            return -1;

        timer_storage_[id] = timer_op;
        timers_.push(timer_op);
        backend_->wake();

        return id;
    }

    void io_context::impl::cancel_timer(timer_id id)
    {
        if (timer_storage_.find(id) != timer_storage_.end())
        {
            auto& ts = timer_storage_[id];
            if (ts)
                ts->canceled = true;
        }
    }

    void io_context::impl::register_reactor_handle(native_handle_t handle, void* token, io_interest interest)
    {
        backend_->add(handle, token, interest);
        backend_->wake();
    }

    void io_context::impl::modify_reactor_handle(native_handle_t handle, void* token, io_interest interest)
    {
        backend_->modify(handle, token, interest);
        backend_->wake();
    }

    void io_context::impl::unregister_reactor_handle(native_handle_t handle, void * token, io_interest interest)
    {
        backend_->remove(handle, this, interest);
        backend_->wake();
    }

    void io_context::impl::drainIncoming()
    {
    }

    void io_context::impl::drainExpiredTimers()
    {
        auto now = clock::now();
        while (!timers_.empty())
        {
            auto & top = timers_.top();

            if (top->canceled) {
                // Always remove cancelled timers so they don't skew the timeout.
                timer_storage_.erase(top->id);
                timers_.pop();
                continue;
            }

            if (top->expiry > now)
                break;

            post(std::move(top->task));
            timer_storage_.erase(top->id);
            timers_.pop();
        }
    }

    std::optional<duration> io_context::impl::computeWaitTimeout(std::optional<duration> max_duration) const
    {
        // Cancelled timers have been drained by drainExpiredTimers, so the top
        // of the heap is always the next active (non-cancelled) timer, or empty.
        std::optional<duration> out;

        if (!timers_.empty()) {
            auto until_next = timers_.top()->expiry - clock::now();
            // Clamp to zero so we never pass a negative duration to the backend.
            out = until_next > duration::zero() ? until_next : duration::zero();
        }

        if (max_duration.has_value())
            out = out.has_value() ? std::min(*out, *max_duration) : max_duration;

        return out;
    }

    std::size_t io_context::impl::executeReady()
    {
        std::deque<task_t> tasks_to_execute;
        std::size_t tasks_processed { 0 };

        {
            std::lock_guard lg(incoming_mutex_);
            std::swap(tasks_to_execute, ready_);
        }

        for (auto& task : tasks_to_execute)
        {
            task();
            tasks_processed++;
        }
        tasks_to_execute.clear();

        return tasks_processed;
    }

    std::size_t io_context::impl::execute_one_ready()
    {
        task_t task_to_execute { nullptr };
        std::size_t tasks_processed { 0 };
        {
            std::lock_guard lg(incoming_mutex_);
            // std::swap(tasks_to_execute, ready_);
            if (!ready_.empty())
            {
                task_to_execute = ready_.front();
                ready_.pop_front();
            }
        }

        if (task_to_execute)
        {
            task_to_execute();
            tasks_processed++;
        }

        return tasks_processed;
    }

    void io_context::impl::processBackendEvents(backend_event* events, std::size_t count)
    {
        for (auto i = 0; i < count; i++)
        {
            if ((events[i].events & io_event::wakeup) != io_event::none)
            {
                auto n = _consume_wakeup_event(&(events[i]));
                continue;
            }

            static_cast<reactor_handle *>(events[i].token)->on_event(events[i]);
        }
    }


#if defined(NX_OS_WINDOWS)
    // IOCP wakeup: already consumed by GetQueuedCompletionStatus – nothing to drain.
    std::size_t io_context::impl::_consume_wakeup_event(backend_event* /*event*/)
    {
        return 1;
    }
#elif defined(NX_OS_LINUX)
#include <unistd.h>
    std::size_t io_context::impl::_consume_wakeup_event(backend_event* event)
    {
        char buf [128];
        auto n = read(event->identity, buf, sizeof(buf));
        return n;
    }
#elif defined(NX_OS_APPLE)
#include <sys/event.h>
    std::size_t io_context::impl::_consume_wakeup_event(backend_event *event) {
        return 1;
    }

#else
# error "Not supported platform"
#endif
}
