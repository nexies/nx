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

std::atomic<bool> stop{false};

void* signal_thread(void*)
{
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGTERM);
    sigaddset(&set, SIGQUIT);

    int sig;
    while (sigwait(&set, &sig) == 0) {
        {
            g_exitSignals.fetch_add(1, std::memory_order_relaxed);
        }
    }
    return nullptr;
}

nx::MainDispatcher::MainDispatcher()
{
    this->_reattachToThread(&thread);
    thread.start();
    thread.pushSignal(Signal::Custom(this, &MainDispatcher::_setThreadMask), 10);
    Connect(&thread, &Thread::aboutToExitSignal,
        this, &MainDispatcher::_onThreadExit);
}

void nx::MainDispatcher::scanForEvents()
{
    // nxDebug("scanForEvents");
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    _scanExitSignal();

    if (scanning)
        _getLocalThread()->pushSignal(Signal::Custom(this, &MainDispatcher::scanForEvents), 0);
}

void nx::MainDispatcher::stopScanning()
{
    scanning = false;
}

void nx::MainDispatcher::_onThreadExit()
{
    nxDebug("MainDispatcher::_onThreadExit");
    stopScanning();
}

void nx::MainDispatcher::_setThreadMask()
{
    // sigset_t sigset;
    // sigemptyset(&sigset);
    // sigaddset(&sigset, SIGINT);
    // sigaddset(&sigset, SIGTERM);
    // sigaddset(&sigset, SIGPIPE);
    // sigaddset(&sigset, SIGSTOP);
    // sigaddset(&sigset, SIGQUIT);

    pthread_t tid;
    pthread_create(&tid, nullptr, &signal_thread, nullptr);
    pthread_detach(tid);
}

void nx::MainDispatcher::_scanExitSignal()
{
    auto exits = g_exitSignals.exchange(0);
    for (int i = 0; i < exits; i++)
    {
        nx::App::Exit(0);
        scanning = false;
    }
}

void nx::MainDispatcher::_installSignalHandlers() {

    // auto install_signal_handler= [] (int signal) {
    //     std::cerr << "installing handler for signal " << signal << std::endl;
    //     std::signal(signal, signal_handler);
    // };
    //
    // for (int i = 0; i < std::ranges::size(g_catch_signals); i++) {
    //     install_signal_handler(g_catch_signals[i]);
    // }
}
