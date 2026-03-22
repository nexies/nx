//
// Created by nexie on 21.03.2026.
//

#include "context_impl.hpp"

// #include <bits/valarray_array.h>

#include "nx/core/asio/backend/backend.hpp"


namespace nx::asio {
    Context::Impl::Impl(std::unique_ptr<Backend> backend)
        : backend_(std::move(backend))
        , stopped_(false)
        , running_(false)
    {
    }

    Context::Impl::~Impl() {
        stop();
    }

    void Context::Impl::post(Task task) {
        std::lock_guard lg(incoming_mutex_);
        ready_.push_back(std::move(task));
        backend_->wake();
    }

    void Context::Impl::dispatch(Task task) {
        if (runningInThisThread())
            task();
        else
            post(std::move(task));
    }

    std::size_t Context::Impl::runOnce() {
    }

    std::size_t Context::Impl::run(std::optional<Duration> duration) {
        running_.store(true, std::memory_order_release);

        {
            std::lock_guard lg {state_mutex_};
            owner_thread_id = std::this_thread::get_id();
        }

        std::size_t tasks_processed { 0 };
        while (!stopped_.load(std::memory_order_acquire)) {
            drainExpiredTimers();
            tasks_processed += executeReady();

            auto timeout = computeWaitTimeout(duration);

            BackendEvent events[64];
            auto event_count = backend_->wait(events, 64, timeout);

            processBackendEvents(events, event_count);
        }
        running_.store(false);
        return tasks_processed;
    }

    std::size_t Context::Impl::poll() {
    }

    std::size_t Context::Impl::pollOnce() {
    }

    void Context::Impl::stop() {
        stopped_.store(true, std::memory_order_release);
        backend_->wake();
    }

    void Context::Impl::restart() {
        stopped_.store(false, std::memory_order_release);
        backend_->wake();
    }

    bool Context::Impl::stopped() const noexcept {
        return stopped_.load(std::memory_order_acquire);
    }

    bool Context::Impl::runningInThisThread() const noexcept {
        if (!running_.load(std::memory_order_acquire))
            return false;
        {
            std::lock_guard lg {state_mutex_};
            return (std::this_thread::get_id() == owner_thread_id);
        }
    }

    nx::TimerId Context::Impl::createTimer(TimePoint expiry, Task task) {
        auto id = next_timer_id_++;
        auto timer_op = std::make_shared<TimerOp>(expiry, task, false, id);

        if (!timer_op)
            return -1;

        timer_storage_[id] = timer_op;
        timers_.push(timer_op);
        backend_->wake();

        return id;
    }

    void Context::Impl::cancelTimer(TimerId id) {
        timer_storage_[id]->canceled = true;
    }

    void Context::Impl::registerReactorHandle(NativeHandle handle, void *token, IOInterest interest) {
    }

    void Context::Impl::modifyReactorHandle(NativeHandle handle, void *token, IOInterest interest) {
    }

    void Context::Impl::unregisterReactorHandle(NativeHandle handle, void *token, IOInterest interest) {
    }

    void Context::Impl::drainIncoming() {
    }

    void Context::Impl::drainExpiredTimers() {
        auto now = Clock::now();
        while (!timers_.empty()) {
            if (timers_.top()->expiry > now)
                break;

            auto & top = timers_.top();
            if (!top->canceled)
                post(std::move(top->task));

            timer_storage_.erase(top->id);
            timers_.pop();
        }
    }

    std::optional<Duration> Context::Impl::computeWaitTimeout(std::optional<Duration> max_duration) const {
        std::optional<Duration> out = std::nullopt;

        if (!timers_.empty())
            out = timers_.top()->expiry - Clock::now();

        if (max_duration.has_value()) {
            out = out.value_or(max_duration.value());
            out = out > max_duration ? max_duration : out;
        }

        return out;
    }

    std::size_t Context::Impl::executeReady() {
        std::deque<Task> tasks_to_execute;
        std::size_t tasks_processed { 0 };

        {
            std::lock_guard lg (incoming_mutex_);
            std::swap(tasks_to_execute, ready_);
        }

        for (auto & task : tasks_to_execute) {
            task();
            tasks_processed++;
        }
        tasks_to_execute.clear();

        return tasks_processed;
    }

    void Context::Impl::processBackendEvents(BackendEvent *events, std::size_t count) {

    }
}
