//
// Created by nexie on 21.03.2026.
//

#include "context_impl.hpp"

#include <bits/valarray_array.h>

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

        std::deque<Task> tasks_to_execute;
        std::size_t tasks_processed { 0 };

        while (!stopped_.load(std::memory_order_acquire)) {
            {
                std::lock_guard lg (incoming_mutex_);
                std::swap(tasks_to_execute, incoming_);
            }

            for (auto & task : tasks_to_execute) {
                task();
                tasks_processed++;
            }
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
        // return detail::g_invalidTimerId;
        return -1;
    }

    void Context::Impl::cancelTimer(TimerId id) {

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
    }

    std::optional<Duration> Context::Impl::computeWaitTimeout(std::optional<Duration> max_duration) const {
    }

    std::size_t Context::Impl::executeReady() {
    }

    void Context::Impl::processBackendEvents(BackendEvent *events, std::size_t count) {
    }
}
