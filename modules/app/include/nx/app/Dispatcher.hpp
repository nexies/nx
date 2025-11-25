//
// Created by nexie on 19.11.2025.
//

#ifndef DISPATCHER_HPP
#define DISPATCHER_HPP

#include "nx/app/Loop.hpp"
#include "nx/app/Thread.hpp"
#include "nx/core/TimerWheel.h"

namespace nx {

    class ListenLoop : public Loop {
    public:
        Result exec() override;
    };

    class Dispatcher : public Thread {
    public:
        Result execute() override;
    protected:
    };

    class MainDispatcher final : public Dispatcher {

    public:
        ~MainDispatcher() override = default;
        Result execute () override;

        TimerId addTimer(TimerType, Duration, detail::timer_callback_t);
        Result cancelTimer(TimerId);

    protected:
        void _installSignalHandlers ();

        void _scanExitSignals ();
        void _rotateTimers ();
        void _scanInputChars ();

    private:
        TimerWheel<1024> timerWheel;
    };

}

#endif //DISPATCHER_HPP
