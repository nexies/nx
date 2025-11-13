//
// Created by nexie on 12.11.2025.
//

#ifndef THREAD_HPP
#define THREAD_HPP

#include "Object.hpp"
#include "Event.hpp"
#include <thread>

namespace nx
{
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

        bool isRunning () const;
        bool isSleeping () const;

        void sleep (Duration);
        void sleepUntil (TimerPoint);

        void exit ();
        void quit ();

    private:
        std::thread thread;
        thread_id id;
        std::atomic_bool running { false };
        std::atomic_bool sleeping { false };
        EventQueue queue;
    };
}

#endif //THREAD_HPP
