//
// Created by nexie on 25.11.2025.
//

#include "nx/app/Timer.hpp"
#include "nx/app/Thread.hpp"

using namespace nx;

Timer::Timer() :
    Timer(Seconds(1), SingleShot)
{

}

Timer::Timer(Duration dur, Type type) :
    Object(),
    dur(dur),
    type(type),
    timer(nullptr),
    reps(0),
    reps_left(0)
{

}

Timer::~Timer()
{
    _unset();
}

bool Timer::start()
{
    return _set();
}

bool Timer::start(Duration dur)
{
    this->dur = dur;
    return _set();
}

void Timer::stop()
{
    _unset();
}

Duration Timer::getDuration() const
{
    return dur;
}

void Timer::setDuration(Duration dur)
{
    if (dur != this->dur)
        durationChanged(dur);

    this->dur = dur;
}

Timer::Type Timer::getType() const
{
    return type;
}

void Timer::setType(Type type)
{
    if (this->type != type)
        typeChanged(type);

    this->type = type;
}

bool Timer::_set()
{
    if (!thread())
    {
        nxError("Try to set timer outside of nx::Thread");
        return false;
    }

    if (timer)
    {
        nxError("Timer is already running");
        return false;
    }

    uint64_t microsecs = std::chrono::duration_cast<std::chrono::microseconds>(dur).count();
    timer = std::make_unique<timer_type>(thread()->context(), boost::posix_time::microseconds(microsecs));
    if (!timer)
    {
        nxError("Failed to set a timer");
        return false;
    }

    reps_left = reps;
    timer->async_wait(boost::bind(&Timer::_OnTimeout, this, boost::placeholders::_1));
    return true;
}

void Timer::_unset()
{
    if (timer)
    {
        timer->cancel();
        timer.reset(nullptr);
    }
    reps_left = 0;
}

void Timer::_OnTimeout(Timer* obj, const boost::system::error_code& e)
{
    obj->timeout();
    obj->_unset();

    switch (obj->type)
    {
    case Timer::Periodic:
        obj->_set();
        break;
    // case Timer::LimitRepetition:
    //     if (obj->reps_left > 0)
    //     {
    //         obj->reps_left--;
    //         obj->_set();
    //         break;
    //     }
    //     else
    //     {
    //         obj->_unset();
    //         break;
    //     }
    case Timer::SingleShot:
        // obj->_unset();
        break;
    }
}
