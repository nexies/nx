//
// Created by nexie on 19.11.2025.
//

#include "app/Dispatcher.hpp"

#include <iostream>

#include "signal.h"
#include "csignal"
#include "app/App.hpp"

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
    _installSignalHandlers();

    while (running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        // nxDebug("Scanning signals");

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
    return Result::Ok();
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
