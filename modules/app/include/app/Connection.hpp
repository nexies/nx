//
// Created by nexie on 09.11.2025.
//

#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <iostream>
#include <memory>

#include "app/Functor.hpp"
#include "app/Signal.hpp"
#include "app/Invoker.hpp"

namespace nx {
    namespace detail
    {
        enum class ConnectionType : int8_t {
            Auto = 0,
            Direct,
            Queued,
            QueuedBlocking,
            QueuedBlockingSingleThread,
        };
        class ConnectionBase
        {
            virtual type () = 0;
        };

        template<typename... Args>
        class ConnectionInput : public ConnectionBase {
        protected:
            virtual void transmit_impl (Args... args) = 0;
        public:
            virtual ~ConnectionInput () = default;
            void transmit (Args... args) {
                transmit_impl (args...);
            }
        };
    }
}
#endif //CONNECTION_HPP
