//
// Created by nexie on 25.11.2025.
//

#include "nx/app/Timer.hpp"


nx::detail::TimerManagerInstance::TimerManagerInstance(boost::asio::io_service& io_service) :
    io_service(io_service),
    timers({})
{

}

size_t nx::detail::TimerManagerInstance::Poll()
{
    auto & self = _Self();
    boost::system::error_code ec;
    auto out = self.io_service.poll(ec);
    if (ec.failed())
        nxError("Failure at boost::asio::io_service: {}", ec.what());
    return out;
}

nx::TimerId nx::detail::TimerManagerInstance::_NextTimerId()
{
    static TimerId current = 0;
    static std::mutex mutex;

    std::lock_guard<std::mutex> lock(mutex);
    do
    {
        current += 1;
    } while (current == g_invalidTimerId);
    return current;
}

nx::detail::TimerManagerInstance& nx::detail::TimerManagerInstance::_Self()
{
    return TimerManager::Instance();
}

nx::Timer::Timer() :
    Object(),
    running(false)
{

}

nx::Timer::~Timer()
{

}
