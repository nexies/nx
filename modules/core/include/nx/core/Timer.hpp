//
// Created by nexie on 25.11.2025.
//

#ifndef TIMER_HPP
#define TIMER_HPP

#include <boost/asio/deadline_timer.hpp>

#include <nx/core/Object.hpp>

namespace nx
{
    class Timer : public Object
    {
    public:
        enum Type
        {
            SingleShot,
            Periodic,
            // LimitRepetition
        };

        Timer();
        explicit Timer(Duration dur, Type type = SingleShot);
        ~Timer() override;

        bool start();
        bool start(Duration dur);
        bool startNow();
        bool startNow(Duration dur);

        void stop ();

        NX_SIGNAL(timeout)

        NX_PROPERTY(TYPE Type, NAME timer_type, READ type, WRITE setType, NOTIFY typeChanged, DEFAULT SingleShot)
        NX_PROPERTY(TYPE Duration, NAME duration, READ duration, WRITE setDuration, NOTIFY durationChanged, DEFAULT Seconds(1))

    private:
        using timer_type = boost::asio::deadline_timer;
        std::unique_ptr<timer_type> timer;

        TimePoint time_started;
        int reps;
        // Type type;
        int reps_left;

        bool _set();
        void _unset();
        static void _OnTimeout(Timer * obj, const boost::system::error_code& e);
    };
}

#endif //TIMER_HPP
