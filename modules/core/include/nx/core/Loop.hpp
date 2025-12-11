//
// Created by nexie on 12.11.2025.
//

#ifndef LOOP_HPP
#define LOOP_HPP

#include <boost/asio/executor_work_guard.hpp>
#include <boost/asio/io_context.hpp>

#include <nx/core/Signal.hpp>
#include <nx/core/Object.hpp>

namespace nx
{
    class Loop : public Object
    {
        friend class Signal;
    public:
        Loop ();
        ~Loop();

        virtual Result exec ();

        virtual Result processEvents ();
        virtual Result processEventsFor (Duration);
        Result processEventsUntil (TimePoint);

        Result exit ();
        Result quit ();

        bool isRunning () const;
        bool isSleeping ();

        void flush ();

        void setWaitDuration (Duration);
        Duration waitDuration () const;

    protected:
        std::atomic_bool running {false};
        std::atomic_bool sleeping {false};
        std::atomic_bool interrupt {false};

        bool _waitForSignals ();
        bool _waitForSignalsFor (Duration);

        // bool _processSingleEntry (SignalQueue::Entry & entry) const;
        // bool _redirectEntry (SignalQueue::Entry & entry) const;

        void _quitImpl ();
        void _exitImpl (int code);
        void _interruptImpl ();

        void _installLoopOntoThread();
        void _uninstallLoopFromThread();

    private:
        // SignalQueue * queue;
        Loop * underlying_loop { nullptr };
        int exit_code { 0 };
        Duration wait_duration { 0 };
    };
}

namespace nx::experimental
{

/// Thoughts on Loop:
///
///     Needs to implement conditional loop on current thread()->context() blocking the execution of the following code.
///         loop.while([&] { return some_local_condition; });
///
///     Executing poll from event pool for some period of time:
///         loop.for(Seconds(10))
///         loop.until(Clock::now() + Seconds(10));
///
///     Default current Loop::loop() method will translate into:
///         loop.while([&] { return this->running; });
///
///     Exiting the current active loop may be caused by:
///         1) Satisfying the condition on which the loop is entered
///         2) Receiving Exit event from the pool
///

    class Loop : public Object
    {
    public:
        enum ExitReason
        {
            ConditionMet,
            ExitEvent,
            Abnormal,
        };

        struct res_ok
        {
            ExitReason reason;
            size_t events_processed;
        };

        using Result = nx::result_t<res_ok, nx::common::common_err_t>;

        using Condition = std::function<bool()>;

        Loop();
        ~Loop();

        Result loop_while(Condition && condition);
        Result loop_forever();
        Result loop_for(Duration dur);
        Result loop_until(TimePoint tp);

    protected:
        bool _checkCondition ();
        bool _hasCondition ();
        Condition _condition;

    };
}

#endif //LOOP_HPP
