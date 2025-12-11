//
// Created by nexie on 25.11.2025.
//

#include "../include/nx/core/Timer.hpp"
#include "../include/nx/core/Thread.hpp"

using namespace nx;

Timer::Timer() :
    Timer(Seconds(1), SingleShot)
{

}

Timer::Timer(Duration dur, Type type) :
    Object(),
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
    setDuration(dur);
    return _set();
}

bool Timer::startNow()
{
    auto ret = start();
    if (ret)
        this->timeout();
    return ret;
}

bool Timer::startNow(Duration dur)
{
    auto ret = start(dur);
    if (ret)
        this->timeout();
    return ret;
}

void Timer::stop()
{
    _unset();
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

    uint64_t microsecs = std::chrono::duration_cast<std::chrono::microseconds>(duration()).count();
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

void Timer::_OnTimeout(Timer* timer, const boost::system::error_code& e)
{
    NX_EMIT(timer->timeout)
    timer->_unset();

    switch (timer->type())
    {
    case Timer::Periodic:
        timer->_set();
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
