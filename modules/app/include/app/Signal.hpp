//
// Created by nexie on 09.11.2025.
//

#ifndef SIGNAL_HPP
#define SIGNAL_HPP

#include "core/Result.hpp"

#define NX_SIGNAL(signalName, ...) \
    void signalName(__VA_ARGS__) \
    {}


namespace nx {
    template<typename SignalType, typename... Args>
    Result emit_signal(SignalType signal, Args&&... args) {
        return Result::Err("Signal mechanism is not yet implemented");

        /// calculate hash by passed arguments?... + signal function address ??
        ///
        /// I CAN DO static_cast<something<Args...>> here.
        /// Connection has to be type-definible by only passed arguments
        /// plan:
        /// - get Functor(s) from Connection object(s)
        ///     - Functor is stored as FunctorBase. Need to cast it to correct type
        /// - create Invoker with Functor and arguments
        /// - pass Invoker into the event loop according to the connection policy

    }
}

#endif //SIGNAL_HPP
