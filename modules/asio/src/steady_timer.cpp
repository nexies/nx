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

    steady_timer::~steady_timer()
    {
        if (running())
            cancel();
    }

    void steady_timer::async_wait(duration dur, task_t task)
    {
        if (running())
            cancel();

        expiry_ = clock::now() + dur;
        // Wrap the task so id_ is cleared after the user callback returns,
        // making running() == false as soon as the callback completes.
        id_ = ctx_.impl_->create_timer(expiry_,
            [this, task = std::move(task)]() mutable {
                task();
                id_ = timer_id::invalid();
            });
    }

    void steady_timer::async_wait(time_point expiry, task_t task)
    {
        if (running())
            cancel();

        expiry_ = expiry;
        id_ = ctx_.impl_->create_timer(expiry_,
            [this, task = std::move(task)]() mutable {
                task();
                id_ = timer_id::invalid();
            });
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
