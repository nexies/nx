//
// Created by nexie on 19.11.2025.
//

#include "nx/app/Dispatcher.hpp"

#include <iostream>

#include <signal.h>
#include "csignal"
#include "nx/app/App.hpp"

std::atomic_uint32_t g_exitSignals{0};
std::atomic_uint32_t g_abortSignals{0};

const int g_catch_signals[] =
{
    SIGINT,
    SIGABRT,
    SIGTERM,
    SIGHUP,
    SIGQUIT,
    // SIGKILL,
    //
    // SIGILL,
    // SIGFPE,
    // SIGSEGV,
};

void signal_handler(int signal) {
    switch (signal) {
        case SIGINT:
        case SIGABRT:
        case SIGTERM:
        case SIGHUP:
        case SIGQUIT:
        case SIGKILL:
            g_exitSignals.fetch_add(1, std::memory_order_release);
            break;
        case SIGILL:
        case SIGFPE:
        case SIGSEGV:
            g_abortSignals.fetch_add(1, std::memory_order_release);
            break;
        default:
            std::cerr << "Received unhandled signal " << signal << std::endl;
    }
}

nx::Result nx::PollService::init() {
    return Result::Ok();
}

nx::Result nx::PollService::cleanup() {
    return Result::Ok();
}

bool nx::PollService::isInit() const {
    return true;
}

bool nx::PollService::initFailed() const {
    return false;
}

size_t nx::PollService::poll() {
    size_t result = 0;
    while (pollOne())
        ++result;
    return result;
}

size_t nx::PollService::pollFor(Duration dur) {
    TimePoint deadline = Clock::now() + dur;
    size_t result = 0;
    while (pollOne() and Clock::now() < deadline) {
        ++result;
    }
    return result;
}

nx::BoostPollService::BoostPollService(boost::asio::io_service &io_service) :
    io_service(io_service)
{

}

bool nx::BoostPollService::pollOne(){
    try {
        io_service.poll_one();
    } catch (boost::system::system_error &e) {
        nxError("BoostPollService::poll_one() failed: {}", e.what());
        return false;
    } catch (std::exception &e) {
        nxError("BoostPollService::poll_one() failed: {}", e.what());
        return false;
    } catch ( ... ) {
        nxError("BoostPollService::poll_one() failed: unknown exception");
        return false;
    }
    return true;

}

std::atomic_int nx::SystemSignalPollService::s_abort_signals { 0 };
std::atomic_int nx::SystemSignalPollService::s_exit_signals { 0 };

void nx::SystemSignalPollService::system_signal_handler(int signal) {
    // std::cerr << "Received signal " << signal << std::endl;
    nxInfo("Received {}", strsignal(signal));
    switch (signal) {
        case SIGINT:
        case SIGABRT:
        case SIGTERM:
        case SIGHUP:
        case SIGQUIT:
        case SIGKILL:
            s_exit_signals.fetch_add(1, std::memory_order_release);
            break;
        case SIGILL:
        case SIGFPE:
        case SIGSEGV:
            s_abort_signals.fetch_add(1, std::memory_order_release);
            break;
        default:
            std::cerr << "Received unhandled signal " << signal << std::endl;
    }
}

nx::Result nx::SystemSignalPollService::_installSignalHandler() {
    for (int i = 0; i < std::ranges::size(g_catch_signals); i++) {
        auto res = std::signal(g_catch_signals[i], system_signal_handler);
        if (res == SIG_ERR) {
            static char errbuf [1024];
            psignal(g_catch_signals[i], errbuf);
            return Result::Err(errbuf);
        }
    }
    return Result::Ok();
}

nx::Result nx::SystemSignalPollService::init() {
    return _installSignalHandler();
}

nx::Result nx::SystemSignalPollService::cleanup() {
    return PollService::cleanup();
}

bool nx::SystemSignalPollService::pollOne() {
    if (s_exit_signals.load(std::memory_order_acquire)) {
        s_exit_signals.fetch_sub(1, std::memory_order_release);
        nx::App::Quit();
        return true;
    }

    if (s_abort_signals.load(std::memory_order_acquire)) {
        s_abort_signals.fetch_sub(1, std::memory_order_release);
        nx::App::Exit(123);
        return true;
    }
    return false;
}

nx::PollLoop::PollLoop(std::set<std::shared_ptr<PollService>>& services) :
    Loop (),
    services(services)
{
    setWaitDuration(Milliseconds(30));
}

nx::Result nx::PollLoop::exec()
{
    nxTrace("PollLoop::exec() start");
    for (auto & service: services)
    {
        nxTrace("PollLoop::exec() service init");
        auto res = service->init();
        if (!res)
        {
            nxError("Failed to initialize service");
            return Result::Err("Failed to initialize service");
        }
    }

    nxTrace("PollLoop: Call to Loop::exec()");
    auto rv = Loop::exec();

    for (auto & service: services)
    {
        nxTrace("PollLoop::exec() service cleanup");
        service->cleanup();
    }
    return rv;
}

nx::Result nx::PollLoop::processEvents() {
    nxTrace("PollLoop::processEvents()");
    auto poll_duration = waitDuration() / services.size();
    auto deadline = Clock::now() + waitDuration();
    for (auto & service: services) {
        nxTrace("PollLoop::processEvents() - service poll");
        service->pollFor(poll_duration);
    }

    auto res = Loop::processEventsUntil(deadline);

    if (running and not interrupt)
        std::this_thread::sleep_until(deadline);

    return res;
}

bool nx::PollLoop::_doServicePoll(std::shared_ptr<PollService>& service, Duration timeout)
{
    if(!service->isInit() && !service->initFailed())
    {
        try
        {
            service->init();
        } catch ( ... )
        {
            return false;
        }
    }
}

nx::PollThread::PollThread() :
    Thread()
{

}

nx::Result nx::PollThread::execute() {
    PollLoop loop (services);
    nxDebug("PollThread start");
    auto res = loop.exec();
    nxDebug("PollThread finish");
    return res;
}

bool nx::PollThread::addService(std::shared_ptr<PollService> service)
{
    if (isRunning())
        return false;

    if (services.contains(service))
        return false;
    services.insert(service);
    return true;

}

bool nx::PollThread::removeService(std::shared_ptr<PollService> service)
{
    if (isRunning())
        return false;

    if (!services.contains(service))
        return false;

    service->cleanup();
    services.erase(service);
    return true;
}

nx::Result nx::MainDispatcher::execute() {
    nxDebug("Start dispatcher");
    _installSignalHandlers();

    while (running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        _scanExitSignals();
        _rotateTimers();
        _scanInputChars();
    }
    return Result::Ok();
}

nx::TimerId nx::MainDispatcher::addTimer(TimerType type, Duration dur, detail::timer_callback_t cb) {
    TimerId out;
    switch (type) {
        case TimerType::SingleShot:
            out = timerWheel.add_singleshot(std::chrono::duration_cast<Milliseconds>(dur), cb);
            break;
        case TimerType::Periodic:
            out = timerWheel.add_periodic(std::chrono::duration_cast<Milliseconds>(dur), cb);
            break;
        default:
            out = detail::invalid_timer;
    }

    return out;
}

nx::Result nx::MainDispatcher::cancelTimer(TimerId id) {
    if (timerWheel.cancel_timer(id))
        return Result::Ok();
    return Result::Err("Failure canceling timer");
}

void nx::MainDispatcher::_installSignalHandlers() {
    auto install_signal_handler = [](int signal) {
        // nxTrace("Installing signal handler for signal {}", signal);
        std::signal(signal, signal_handler);
    };

    for (int i = 0; i < std::ranges::size(g_catch_signals); i++) {
        install_signal_handler(g_catch_signals[i]);
    }
}

void nx::MainDispatcher::_scanExitSignals() {
    if (g_exitSignals > 0) {
        nxDebug("Received exit signal");
        int count = g_exitSignals.exchange(0);
        for (int i = 0; i < count; i++) {
            nx::App::Quit();
            running.store(false);
        }
    }
}

void nx::MainDispatcher::_rotateTimers() {
    thread_local TimePoint lastTime = Clock::now();
    auto now = Clock::now();
    timerWheel.process_time_elapsed(now - lastTime);
    lastTime = now;
}

void nx::MainDispatcher::_scanInputChars() {
}
