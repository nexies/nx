//
// Created by nexie on 09.11.2025.
//

#ifndef SIGNAL_HPP
#define SIGNAL_HPP

#include <queue>
#include <atomic>
#include <condition_variable>

#include <nx/core/detail/signal_defs.hpp>
#include <nx/core/detail/units.hpp>
#include <nx/core/Invoker.hpp>
#include <nx/core/Result.hpp>

namespace nx {
    class Thread;
    class Loop;
    class Object;

    class Signal
    {
        InvokerPtr invoker;
        Object * sender { nullptr };
        Object * receiver { nullptr };

    public:
        Signal (Object * sender, Object * receiver, InvokerPtr invoker);
        Signal(Signal const&) = default;
        Signal& operator=(Signal const&) = default;

        explicit Signal(Signal&& other) noexcept
        {
            *this = std::move(other);
        }
        Signal& operator=(Signal&& other) noexcept
        {
            invoker = std::move(other.invoker);
            receiver = other.receiver;
            sender = other.sender;
            return *this;
        }

        Signal ();

        template<typename... Args>
        Signal(Object * s, FunctorPtr functor, Args... args) : Signal(s, nullptr, nx::make_invoker(functor, args...)) {}

        template<typename... Args>
        Signal(Object * s, Object * o, FunctorPtr functor, Args... args) : Signal (s, o, nx::make_invoker(functor, args...)) {}

        template<typename Ret, typename... Args>
        explicit Signal(Object * s, Ret(*f)(Args...), Args... args) : Signal(s,nullptr, nx::make_invoker(f, args...)) {}

        template<typename Class, typename Ret, typename... Args, typename... Params, std::enable_if_t<std::is_base_of<Object, Class>::value, int> = 0>
        Signal(Object * s, Class* c, Ret(Class::*f)(Args...), Params&&... params) : Signal(s, c, nx::make_invoker(c, f, std::forward<Args>(params)...)) {}

        template<typename Class, typename... Args, std::enable_if_t<not std::is_base_of<Object, Class>::value, int> = 0>
        explicit Signal(Object * s, Class* c, Args... args) : Signal(s, nullptr, nx::make_invoker(c, args...)) {}

        [[nodiscard]]
        ThreadId destinationThreadId () const;

        void activate () const;

        static Signal Quit (Object * sender, Loop * loop);
        static Signal Exit (Object * sender, Loop * loop, int code);
        static Signal Sleep (Object * sender, Thread * thread, int ms);
        static Signal Sleep (Object * sender, Thread * thread, Duration dur);
        static Signal SleepUntil (Object * sender, Thread * thread, TimePoint tp);
        static Signal Interrupt (Object * sender, Loop * loop);
        static Signal NullSignal ();
        static Signal Timer (Object * sender, Object * object, TimerId);

        template<typename... Args>
        static Signal Custom(Args... args);

        template<typename Class, typename... Args, std::enable_if_t<std::is_base_of<Object, Class>::value, int> = 0>
        static Signal Custom(Object * sender, Class* c, Args... args);

        template<typename Class, typename... Args, std::enable_if_t<not std::is_base_of<Object, Class>::value, int> = 0>
        static Signal Custom(Object * sender, Class* c, Args... args);

        void operator () () const;
    };

    template <typename ... Args>
    Signal Signal::Custom(Args... args)
    {
        return Signal(nullptr, nullptr, nx::make_invoker(args...));
    }

    template <typename Class, typename ... Args, std::enable_if_t<std::is_base_of<Object, Class>::value, int>>
    Signal Signal::Custom(Object * sender, Class* c, Args... args)
    {
        return Signal(sender, c, nx::make_invoker(c, args...));
    }

    template <typename Class, typename ... Args, std::enable_if_t<!std::is_base_of<Object, Class>::value, int>>
    Signal Signal::Custom(Object * sender, Class* c, Args... args)
    {
        return Signal(sender, nullptr, nx::make_invoker(c, args...));
    }

    template<typename Sender, typename Signal, typename Receiver, typename Slot>
    bool connect (Sender * sender, Signal && signal, Receiver * receiver, Slot && slot, uint8_t flags);

    template<typename Sender, typename Signal, typename ... Args>
    void emit (Sender * sender, Signal signal, Args&&...);

    // class SignalQueue {
    // public:
    //     struct Entry
    //     {
    //         int priority { 0 };
    //         Signal signal;
    //         bool operator < (const Entry& other) const { return priority < other.priority; }
    //
    //     };
    //     using queue_type = std::priority_queue<Entry>;
    //
    //     SignalQueue (size_t max_size);
    //     ~SignalQueue();
    //
    //     Entry getNext ();
    //     bool pushSignal (Signal && signal, int priority);
    //     bool waitForSignals ();
    //     bool waitForSignals (Duration dur);
    //
    //     size_t count () const;
    //     bool hasPendingSignals () const;
    //
    // private:
    //     queue_type queue { };
    //     std::mutex mutex { };
    //     std::condition_variable cv { };
    //     std::atomic_bool accepting_signals { true };
    //     size_t max_size { 0 };
    // };
    //
    //
    // template<typename SignalType, typename... Args>
    // Result emit_signal(SignalType signal, Args&&... args) {
    //     return Result::Err("Signal mechanism is not yet implemented");
    //
    //     /// calculate hash by passed arguments?... + signal function address ??
    //     ///
    //     /// I CAN DO static_cast<something<Args...>> here.
    //     /// Connection has to be type-definible by only passed arguments
    //     /// plan:
    //     /// - get Functor(s) from Connection object(s)
    //     ///     - Functor is stored as FunctorBase. Need to cast it to correct type
    //     /// - create Invoker with Functor and arguments
    //     /// - pass Invoker into the event loop according to the connection policy
    // }
}

#endif //SIGNAL_HPP
