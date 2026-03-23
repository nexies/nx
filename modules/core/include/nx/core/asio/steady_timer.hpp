//
// Created by nexie on 23.03.2026.
//

#ifndef NX_CORE_ASIO_STEADY_TIMER_HPP
#define NX_CORE_ASIO_STEADY_TIMER_HPP

#include <nx/core/asio/context.hpp>

namespace nx::asio
{
    class SteadyTimer
    {
    public:
        using Task = Context::Task;
        using Duration = Duration;
        using TimePoint = TimePoint;

    private:
        Context & ctx_;
        TimerId id_;
        TimePoint expiry_;

    public:
        explicit
        SteadyTimer(Context & ctx);

        void
        asyncWait(Duration duration, Task task);
        void
        asyncWait(TimePoint expiry, Task task);

        void cancel();

        Duration timeLeft() const;

        TimerId timerId() const;

        bool
        running () const;
    };
}

#endif //NX_CORE_ASIO_STEADY_TIMER_HPP