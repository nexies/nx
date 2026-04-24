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
        using task_t = io_context::task_t;
        using duration = duration;
        using time_point = time_point;
        using clock_t = clock;

    private:
        io_context & ctx_;
        timer_id id_;
        time_point expiry_;

    public:
        NX_DISABLE_COPY(steady_timer)

        explicit steady_timer(io_context & ctx);
        ~steady_timer();

        void
        async_wait(duration duration, task_t task);
        void
        async_wait(time_point expiry, task_t task);

        void cancel();

        NX_NODISCARD duration
        time_left() const;

        NX_NODISCARD timer_id
        id() const;

        NX_NODISCARD bool
        running () const;

    };
}

#endif //NX_CORE_ASIO_STEADY_TIMER_HPP