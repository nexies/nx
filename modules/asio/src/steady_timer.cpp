//
// Created by nexie on 23.03.2026.
//

#include <nx/asio/steady_timer.hpp>

#include "context/context_impl.hpp"
#include "nx/common/types/errors/codes.hpp"

namespace nx::asio
{
    steady_timer::steady_timer(io_context& ctx)
        : ctx_(ctx)
        , id_(timer_id::invalid())
    {
    }

    void steady_timer::async_wait(duration duration, task_t task)
    {
        if (running())
            throw nx::err::invalid_state("SteadyTimer::asyncWait - timer is already running");

        expiry_ = clock::now() + duration;
        id_ = ctx_.impl_->create_timer(expiry_, std::move(task));
    }

    void steady_timer::async_wait(time_point expiry, task_t task)
    {
        if (running())
            throw nx::err::invalid_state("SteadyTimer::asyncWait - timer is already running");

        expiry_ = expiry;
        id_ = ctx_.impl_->create_timer(expiry_, std::move(task));
    }

    void steady_timer::cancel()
    {
        ctx_.impl_->cancel_timer(id_);
        id_ = timer_id::invalid();
    }

    duration steady_timer::time_left() const
    {
        if (running())
            return expiry_ - clock::now();

        return duration::zero();
    }

    timer_id steady_timer::id() const
    {
        return id_;
    }

    bool steady_timer::running() const
    {
        return id_ != timer_id::invalid();
    }
}
