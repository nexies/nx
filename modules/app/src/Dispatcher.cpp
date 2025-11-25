//
// Created by nexie on 19.11.2025.
//

#include "nx/app/Dispatcher.hpp"

#include <iostream>

#include <signal.h>
#include "csignal"
#include "nx/app/App.hpp"

std::atomic_uint32_t g_exitSignals { 0 };
std::atomic_uint32_t g_abortSignals { 0 };

const int g_catch_signals [] =
    {
        SIGINT,
        SIGABRT,
        SIGTERM,
        SIGHUP,
        SIGQUIT,
        SIGKILL,

        SIGILL,
        SIGFPE,
        SIGSEGV,
    };

void signal_handler (int signal) {
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

nx::Result nx::ListenLoop::exec() {
    return Loop::exec();
}

nx::Result nx::Dispatcher::execute() {
    // do pre-main loop procedures

    // enter main loop
    // while (running)
    //      iterate through EventListeners
    //      react to each event in a corresponding routine
    //      generate signal according the event


    return Thread::execute();
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

nx::TimerId nx::MainDispatcher::addTimer(TimerType type, Duration dur, detail::timer_callback_t cb)
{
    TimerId out;
    switch (type)
    {
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

nx::Result nx::MainDispatcher::cancelTimer(TimerId id)
{
    if (timerWheel.cancel_timer(id))
        return Result::Ok();
    return Result::Err("Failure canceling timer");
}

void nx::MainDispatcher::_installSignalHandlers() {

    auto install_signal_handler= [] (int signal) {
        // nxTrace("Installing signal handler for signal {}", signal);
        std::signal(signal, signal_handler);
    };

    for (int i = 0; i < std::ranges::size(g_catch_signals); i++) {
        install_signal_handler(g_catch_signals[i]);
    }
}

void nx::MainDispatcher::_scanExitSignals()
{
    if (g_exitSignals > 0) {
        nxDebug("Received exit signal");
        int count = g_exitSignals.exchange(0);
        for (int i = 0; i < count; i++)
        {
            nx::App::Quit();
            running.store(false);
        }
    }
}

void nx::MainDispatcher::_rotateTimers()
{
    thread_local TimePoint lastTime = Clock::now();
    auto now = Clock::now();
    timerWheel.process_time_elapsed(now - lastTime);
    lastTime = now;
}

void nx::MainDispatcher::_scanInputChars()
{

}
