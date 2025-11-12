//
// Created by nexie on 09.11.2025.
//

#ifndef EVENTLOOP_HPP
#define EVENTLOOP_HPP

#include <deque>
#include <queue>
#include <unordered_map>
#include <bits/this_thread_sleep.h>

#include "core/Singleton.hpp"
#include "core/Result.hpp"
#include "app/Event.hpp"
#include <thread>

#include "nxapp.hpp"

namespace nx {

    class EventLoop;
    class EventLoop {
    public:
        enum class State
        {
            Created,
            Starting,
            Started,
            Paused,
            Exiting,
            Exited,
            Destroyed,
        };

        EventLoop();
        virtual ~EventLoop();

        void addEvent (Event * ev, int priority = 0);
        Result processEvents();
        Result processEventsFor(Duration dur);
        Result processEventsUntil(TimerPoint tp);
        Result exec ();

        void exit ();

    protected:
        struct EventEntry
        {
            int priority { 0 };
            Event * event { nullptr };
            bool operator < (const EventEntry & other) const { return priority < other.priority; }
        };
        std::priority_queue<EventEntry> _events;
        std::mutex _mutex;

        std::thread::id _exec_thread_id;
        bool _should_exit { false };

        bool _has_events { false };
        std::condition_variable _has_events_cv;


        void notify_dispatcher (State);
        EventEntry next_entry ();
        Result process_single_event ();
        void wait_for_events ();

    };

    class EventLoopDispatcherInstance
    {
        using id = std::thread::id;
        std::unordered_map<id, EventLoop*> _d;
        std::mutex _m;
        EventLoop* _main { nullptr };
    public:
        void onEventLoopStateChanged (id, EventLoop *, EventLoop::State);
        void addMainEventLoop(id, EventLoop *);
        void addEventLoop(id, EventLoop *);
        void removeEventLoop(id);
        EventLoop * getEventLoop(id);
        void setLoopState(id, EventLoop::State);
        EventLoop * mainEventLoop ();

        EventLoopDispatcherInstance() {};
    };
    using EventLoopDispatcher = Singleton<EventLoopDispatcherInstance>;
}


#endif //EVENTLOOP_HPP
