//
// Created by nexie on 09.11.2025.
//

#ifndef EVENTLOOP_HPP
#define EVENTLOOP_HPP

#include <deque>
#include <unordered_map>
#include <bits/this_thread_sleep.h>

#include "core/Singleton.hpp"
#include <thread>

namespace nx {
    class EventLoop;
    class EventLoopDispatcherInstance
    {
        using id = std::thread::id;
        std::unordered_map<id, EventLoop*> _d;
        std::mutex _m;
    public:
        void addEventLoop(id, EventLoop *);
        void removeEventLoop(id);
        EventLoop * getEventLoop(id);
    };
    using EventLoopDispatcher = Singleton<EventLoopDispatcherInstance>;

    class EventLoop {
        void process();

    };
}


#endif //EVENTLOOP_HPP
