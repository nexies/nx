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
    namespace detail {
        enum class ConnectionType : int8_t {
            Auto = 0,
            Direct,
            Queued,
            QueuedBlocking,
            QueuedBlockingSingleThread,
        };
        class ConnectionBase { };

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

        template<ConnectionType Type>
        struct Transmitter;

        template<>
        struct Transmitter<ConnectionType::Direct> {
            static void transmit (InvokerPtr invoker, size_t dest_thread_id = 0) {
                invoker->invoke ();
            }
        };

        template<>
        struct Transmitter<ConnectionType::Queued> {
            static void transmit (InvokerPtr invoker, size_t dest_thread_id = 0) {
                std::cerr << "Transmitter<Queued> is not implemented yet";
            }
        };

        template<>
        struct Transmitter<ConnectionType::QueuedBlocking> {
            static void transmit (InvokerPtr invoker, size_t dest_thread_id = 0) {
                std::cerr << "Transmitter<QueuedBlocking> is not implemented yet";
            }
        };
        template<>
        struct Transmitter<ConnectionType::Auto> {
            static void transmit (InvokerPtr invoker, size_t dest_thread_id = 0) {
                std::cerr << "Transmitter<Auto> is not implemented yet";
            }
        };

    }

    template<detail::ConnectionType Type, typename... Args>
    class Connection : public detail::ConnectionInput<Args...> {
        using Kind = detail::ConnectionType;
        using Transmitter = detail::Transmitter<Type>;
        using FunctorInput = detail::FunctorInput<Args...>;
        FunctorPtr _functor;
        size_t _dest_thread_id;

    protected:
        void transmit_impl (Args... args) override {
            auto invoker = nx::make_invoker(_functor, args...);
            Transmitter::transmit(invoker, _dest_thread_id);
        }

    public:
        explicit Connection (size_t dest_thread_id, FunctorPtr functor) :
            _functor (std::move(functor)),
            _dest_thread_id (dest_thread_id)
        {}
    };

    template<detail::ConnectionType Type, typename ... Args>
    Connection(Args...) -> Connection<Type, Args...>;

    typedef std::shared_ptr<detail::ConnectionBase> ConnectionPtr;

    template<typename... Args>
    ConnectionPtr make_connection (detail::ConnectionType type, Args... args) {
        switch (type) {
            case detail::ConnectionType::Direct:
                return new Connection<detail::ConnectionType::Direct, Args...>(args...);
            case detail::ConnectionType::Queued:
                return new Connection<detail::ConnectionType::Queued, Args...>(args...);
            case detail::ConnectionType::QueuedBlocking:
                return new Connection<detail::ConnectionType::QueuedBlocking, Args...>(args...);
            case detail::ConnectionType::QueuedBlockingSingleThread:
                return new Connection<detail::ConnectionType::QueuedBlockingSingleThread, Args...>(args...);
            case detail::ConnectionType::Auto:
                return new Connection<detail::ConnectionType::Auto, Args...>(args...);
        }
    }

}
#endif //CONNECTION_HPP
