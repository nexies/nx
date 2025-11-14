//
// Created by nexie on 12.11.2025.
//

#ifndef EVENT_HPP
#define EVENT_HPP
#include <condition_variable>
#include <queue>

#include "../include/app/Signal.hpp"
#include "../include/app/nxapp.hpp"

namespace nx {

    class Object;
    class GenericEvent
    {
    public:
        static GenericEvent Update ();
        static GenericEvent Break ();
        static GenericEvent Quit ();
        static GenericEvent Exit (int code);
        static GenericEvent Sleep (int ms);
        static GenericEvent Sleep (Duration dur);
        static GenericEvent SleepUntil (TimePoint tp);
        static GenericEvent Interrupt ();
        static GenericEvent Custom (InvokerPtr invoker);

        ThreadId destThread () const;
        void accept () const;

    private:
        Signal signal;

        explicit GenericEvent (InvokerPtr && invoker);
        GenericEvent (Object * dest, InvokerPtr && invoker);
    };
    //
    //
    // class Event
    // {
    // public:
    //     enum Type
    //     {
    //         None,
    //         Timer,
    //         Startup,
    //         Exit,
    //         Quit,
    //         Signal,
    //         Update,
    //         Sleep,
    //     };
    //
    //     explicit Event(Type type);
    //     virtual ~Event() {}
    //     inline int32_t type () const { return _type; }
    //
    //     virtual void accept ();
    // private:
    //     int32_t _type { None };
    //     bool _accepted { false };
    // };
    //
    // class SignalEvent : public Event
    // {
    // public:
    //     SignalEvent(ThreadId, InvokerPtr);
    //     void accept () override;
    //     std::thread::id targetThread () const;
    //
    // private:
    //     ThreadId _thread_id;
    //     InvokerPtr _invoker;
    // };
    //
    // class TimerEvent : public Event
    // {
    // public:
    //     TimerEvent(int timer_id);
    // };
    //
    // class SleepEvent : public Event
    // {
    //     Duration duration;
    // public:
    //     SleepEvent(Duration);
    //     SleepEvent(TimePoint);
    //
    //     inline Duration getDuration () const { return duration; }
    //     inline TimePoint getTimePoint () const { return Clock::now() + duration; }
    // };

    class Object;
    // class EventQueue {
    // public:
    //     struct Entry
    //     {
    //         int priority { 0 };
    //         Object * receiver { nullptr };
    //         Event * event { nullptr };
    //         bool operator < (const Entry& other) const { return priority < other.priority; }
    //     };
    //     using queue_type = std::priority_queue<Entry>;
    //
    //     EventQueue (size_t max_size);
    //     ~EventQueue();
    //
    //     Entry getNext ();
    //     bool pushEvent (Object * object, Event * event, int priority);
    //     bool waitForEvents ();
    //     bool waitForEvents (Duration dur);
    //
    //     size_t count () const;
    //     bool hasPendingEvents () const;
    //
    // private:
    //     queue_type queue { };
    //     std::mutex mutex { };
    //     std::condition_variable cv { };
    //     std::atomic_bool accepting_events { true };
    //     size_t max_size { 0 };
    // };
}

#endif //EVENT_HPP
