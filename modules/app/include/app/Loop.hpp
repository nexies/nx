//
// Created by nexie on 12.11.2025.
//

#ifndef LOOP_HPP
#define LOOP_HPP

#include "Signal.hpp"
#include "Object.hpp"

namespace nx
{
    class Loop : public Object
    {
        friend class Signal;
    public:
        Loop ();
        ~Loop();
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

        bool _waitForSignals ();
        bool _waitForSignalsFor (Duration);

        bool _processSingleEntry (SignalQueue::Entry & entry) const;
        bool _redirectEntry (SignalQueue::Entry & entry) const;

        void _quitImpl ();
        void _exitImpl (int code);
        void _interruptImpl ();

    private:
        SignalQueue * queue;
        Loop * underlying_loop { nullptr };
        int exit_code { 0 };
    };
}

#endif //LOOP_HPP
