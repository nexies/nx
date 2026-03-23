//
// Created by nexie on 23.03.2026.
//

#include <nx/core/asio/steady_timer.hpp>

#include "context/context_impl.hpp"

namespace nx::asio
{
    SteadyTimer::SteadyTimer(Context& ctx)
        : ctx_(ctx)
        , id_(g_invalidTimerId)
    {
    }

    void SteadyTimer::asyncWait(Duration duration, Task task)
    {
        if (running())
            throw std::runtime_error("SteadyTimer::asyncWait - timer is already running");

        expiry_ = Clock::now() + duration;
        id_ = ctx_.impl_->createTimer(expiry_, task);
    }

    void SteadyTimer::asyncWait(TimePoint expiry, Task task)
    {
        if (running())
            throw std::runtime_error("SteadyTimer::asyncWait - timer is already running");

        expiry_ = expiry;
        id_ = ctx_.impl_->createTimer(expiry_, task);
    }

    void SteadyTimer::cancel()
    {
        ctx_.impl_->cancelTimer(id_);
        id_ = g_invalidTimerId;
    }

    Duration SteadyTimer::timeLeft() const
    {
        if (running())
            return expiry_ - Clock::now();

        return Duration::zero();
    }

    TimerId SteadyTimer::timerId() const
    {
        return id_;
    }

    bool SteadyTimer::running() const
    {
        return id_ != g_invalidTimerId;
    }
}
