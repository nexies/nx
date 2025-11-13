//
// Created by nexie on 12.11.2025.
//

#ifndef LOOP_HPP
#define LOOP_HPP

#include "Event.hpp"
#include "Object.hpp"

namespace nx
{
    class Loop : public Object
    {
    public:
        Loop ();
        Result exec ();

        Result processEvents ();
        Result processEventsFor (Duration);
        Result processEventsUntil (TimePoint);

        Result exit ();
        Result quit ();

        bool isRunning () const;
        bool isSleeping ();

        void flush ();

    protected:
        std::atomic<bool> running;
        std::atomic<bool> sleeping;
        std::atomic<bool> interrupt;

        bool _waitForEvents ();
        bool _waitForEventsFor (Duration);

        bool _processSingleEntry (EventQueue::Entry & entry);
        bool _redirectEntry (EventQueue::Entry & entry);

        void _quitImpl ();
        void _exitImpl ();
    private:
        EventQueue * queue;
    };
}

#endif //LOOP_HPP
