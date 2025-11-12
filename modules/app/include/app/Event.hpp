//
// Created by nexie on 12.11.2025.
//

#ifndef EVENT_HPP
#define EVENT_HPP
#include <cstdint>
#include <thread>

#include "Invoker.hpp"

namespace nx
{
    class Event
    {
    public:
        enum Type
        {
            None,
            Timer,
            Startup,
            Quit,
            Signal,
        };

        explicit Event(Type type);
        virtual ~Event() {}
        inline int32_t type () const { return _type; }

        virtual void accept ();
    private:
        int32_t _type { None };
        bool _accepted { false };
    };

    class SignalEvent : public Event
    {
    public:
        SignalEvent(std::thread::id, InvokerPtr);
        void accept () override;
        std::thread::id targetThread () const;

    private:
        std::thread::id _thread_id;
        InvokerPtr _invoker;
    };
}

#endif //EVENT_HPP
