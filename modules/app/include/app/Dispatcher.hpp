//
// Created by nexie on 19.11.2025.
//

#ifndef DISPATCHER_HPP
#define DISPATCHER_HPP

#include "Loop.hpp"
#include "app/Thread.hpp"

namespace nx {

    class MainDispatcher final : public Object {

    public:
        MainDispatcher();
        ~MainDispatcher() {}

        void scanForEvents();
        void stopScanning ();


    protected:
        bool scanning { true };

        void _onThreadExit ();
        void _setThreadMask ();

        void _scanExitSignal();
        void _installSignalHandlers ();
        Thread thread;
    };

}

#endif //DISPATCHER_HPP
