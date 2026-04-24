//
// Created by nexie on 21.03.2026.
//

#include <nx/asio/context/io_context.hpp>
#include "nx/asio/backend/backend.hpp"
#include "context_impl.hpp"

namespace nx::asio {
    io_context::io_context()
        : impl_ { std::make_unique<impl>(create_backend()) }
    {
    }

    io_context::~io_context() {
    }

    void io_context::post(task_t task) {
        impl_->post(std::move(task));
    }

    void io_context::dispatch(task_t task) {
        impl_->dispatch(std::move(task));
    }

    std::size_t io_context::run() {
        return impl_->run(std::nullopt);
    }

    std::size_t io_context::run_once() {
        return impl_->poll_once();
    }

    std::size_t io_context::run_for(duration duration) {
        return impl_->run(duration);
    }

    std::size_t io_context::run_until(time_point time_point) {
        return impl_->run(time_point - clock::now());
    }

    std::size_t io_context::poll() {
        return impl_->poll();
    }

    std::size_t io_context::poll_once() {
        return impl_->poll_once();
    }

    void io_context::stop() {
        impl_->stop();
    }

    void io_context::restart() {
        impl_->restart();
    }

    bool io_context::stopped() const noexcept {
        return impl_->stopped();
    }

    bool io_context::is_running_in_this_thread() const noexcept {
        return impl_->is_running_in_this_thread();
    }
}
