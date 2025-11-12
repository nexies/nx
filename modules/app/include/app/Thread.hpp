//
// Created by nexie on 12.11.2025.
//

#ifndef THREAD_HPP
#define THREAD_HPP

#include "Object.hpp"
#include "Event.hpp"
#include <thread>
#include <queue>

namespace nx
{
    namespace detail
    {
        class EventEntry
        {
            int priority { 0 };
            Object * receiver { nullptr };
            Event * event { nullptr };

            bool operator < (const EventEntry& other) const { return priority < other.priority; }
        };
    }

    class Thread : public Object
    {
    public:
        using thread_id = size_t;
        using native_id = std::thread::native_handle_type;

        Thread();

        Result start ();
        virtual Result execute ();

        thread_id getId () const;
        native_id getNativeId () const;

        // bool pushEvent (Object * receiver, Event * )

    private:
        std::thread thread;
        thread_id id;
        std::atomic_bool running { false };
        std::priority_queue<detail::EventEntry> event_queue;

    };
}

#endif //THREAD_HPP
