//
// Created by nexie on 19.11.2025.
//

#ifndef DISPATCHER_HPP
#define DISPATCHER_HPP

#include "Loop.hpp"
#include "app/Thread.hpp"

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
    protected:
        void _installSignalHandlers ();
    };

}

#endif //DISPATCHER_HPP
