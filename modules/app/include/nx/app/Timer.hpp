//
// Created by nexie on 25.11.2025.
//

#ifndef TIMER_HPP
#define TIMER_HPP

#include <boost/asio/deadline_timer.hpp>

#include "nx/app/Object.hpp"

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

        void stop ();

        Duration getDuration () const;
        void setDuration (Duration dur);

        Type getType() const;
        void setType(Type type);


        NX_SIGNAL(timeout)
        NX_SIGNAL(durationChanged, Duration)
        NX_SIGNAL(typeChanged, Type)

    private:
        using timer_type = boost::asio::deadline_timer;
        std::unique_ptr<timer_type> timer;

        Duration dur;
        TimePoint time_started;
        int reps;
        Type type;
        int reps_left;

        bool _set();
        void _unset();
        static void _OnTimeout(Timer * obj, const boost::system::error_code& e);
    };
}

#endif //TIMER_HPP
