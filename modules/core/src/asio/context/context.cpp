//
// Created by nexie on 21.03.2026.
//

#include <nx/core/asio/context/context.hpp>
#include "nx/core/asio/backend/backend.hpp"
#include "context_impl.hpp"

namespace nx::asio {
    Context::Context()
        : impl_ { std::make_unique<Impl>(Backend::CreateBackend()) }
    {
    }

    Context::~Context() {
    }

    void Context::post(Task task) {
        impl_->post(std::move(task));
    }

    void Context::dispatch(Task task) {
        impl_->dispatch(std::move(task));
    }

    std::size_t Context::run() {
        return impl_->run();
    }

    std::size_t Context::runOnce() {
        return impl_->runOnce();
    }

    std::size_t Context::runFor(Duration duration) {
        return impl_->runFor(duration);
    }

    std::size_t Context::runUntil(TimePoint time_point) {
        return impl_->runFor(time_point - Clock::now());
    }

    std::size_t Context::poll() {
        return impl_->poll();
    }

    std::size_t Context::pollOnce() {
        return impl_->pollOnce();
    }

    void Context::stop() {
        impl_->stop();
    }

    void Context::restart() {
        impl_->restart();
    }

    bool Context::stopped() const noexcept {
        return impl_->stopped();
    }

    bool Context::runningInThisThread() const noexcept {
        return impl_->runningInThisThread();
    }
}
