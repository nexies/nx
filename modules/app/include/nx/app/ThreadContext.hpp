//
// Created by nexie on 28.11.2025.
//

#ifndef THREADCONTEXT_HPP
#define THREADCONTEXT_HPP

#include <stack>
#include <unordered_set>

#include "nx/app.hpp"

#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_context.hpp>

#include "Connection.hpp"

namespace nx
{
    class Object;
    using IOService = boost::asio::io_context;

    namespace detail
    {
        using ObjectSet = std::unordered_set<Object *>;
        using ContextWork = std::variant<std::function<void()>, Signal>;
    }

    class ThreadContext
    {
        friend class Object;

        IOService io_context;
        ThreadId id;
        ThreadHandle thread_handle;

        Duration update_period;
        detail::ObjectSet objects;
        ConnectionInfo connections;

        std::stack<Loop *> loops;

        void _notifyObjectCreated (Object * object);
        void _notifyObjectDestroyed (Object * object);
        bool _notifyConnectionCreated (Connection && connection);
        bool _notifyConnectionDestroyed (Connection && connection);

        static void _OnSignal (ThreadContext * self, Signal && signal);

    public:
        ThreadContext();
        ThreadContext(const ThreadHandle & thread_handle);

        IOService & getService();

        void schedule (std::function<void()> && work);
        void schedule (Signal && signal);

    };
}

#endif //THREADCONTEXT_HPP
