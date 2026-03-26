//
// Created by nexie on 23.03.2026.
//

#ifndef NX_CORE_ASIO_STEADY_TIMER_HPP
#define NX_CORE_ASIO_STEADY_TIMER_HPP

#include <nx/asio/context/io_context.hpp>

#include "nx/common/helpers.hpp"

namespace nx::asio
{
    class steady_timer
    {
    public:
        using Task = io_context::task_t;
        using Duration = duration;
        using TimePoint = time_point;

    private:
        io_context & ctx_;
        timer_id id_;
        TimePoint expiry_;

    public:
        explicit
        steady_timer(io_context & ctx);

        void
        asyncWait(Duration duration, Task task);
        void
        asyncWait(TimePoint expiry, Task task);

        void cancel();

        NX_NODISCARD Duration
        timeLeft() const;

        NX_NODISCARD timer_id
        timerId() const;

        NX_NODISCARD bool
        running () const;
    };
}

#endif //NX_CORE_ASIO_STEADY_TIMER_HPP