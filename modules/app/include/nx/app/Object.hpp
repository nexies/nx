//
// Created by nexie on 09.11.2025.
//

#ifndef OBJECT_HPP
#define OBJECT_HPP

#include "nx/app.hpp"
#include "Signal.hpp"
#include "Connection.hpp"
#include "nx/core/Overload.h"

namespace nx {

    class Thread;
    class ConnectionInfo;

    class Object {
        friend class ConnectionInfo;
    public:
        Object ();
        virtual ~Object ();

        [[nodiscard]]
        ThreadId attachedThreadId () const;
        [[nodiscard]]
        Thread * attachedThread () const;

        Result attachToThread (Thread *);

        template<typename EventType>
        Result event (EventType &);

    protected:
        void _generateSignal (Signal && signal, int priority = 0) const;
        Thread * _getLocalThread () const;
        void _reattachToLocalThread ();
        void _reattachToThread (Thread *);

        ConnectionInfo * _getConnectionInfo () const;
    private:
        Thread * local_thread;
        std::unique_ptr<ConnectionInfo> connection_info;

    public:
        template<typename Sender, typename Signal, typename Receiver, typename Slot>
        static bool Connect (Sender * sender, Signal && signal, Receiver * receiver, Slot && slot, uint8_t flags = Connection::Auto);

        template<typename Sender, typename Signal, typename ... Args>
        static void Emit (Sender * sender, Signal signal, Args&&...);
    };

    template <typename EventType>
    Result Object::event(EventType&)
    {
        return Result::Err("Not supported event type");
    }

#define void_cast(val) reinterpret_cast<void *>(val)

    template<typename Sender, typename Signal, typename Receiver, typename Slot>
    bool Object::Connect (Sender * sender, Signal && signal, Receiver * receiver, Slot && slot, uint8_t flags)
    {
        Functor sig_func (sender, signal);
        Functor slot_func (receiver, slot);
        static_assert(std::is_same<typename decltype(slot_func)::ArgsTuple, typename decltype(sig_func)::ArgsTuple>::value, "signal and slot functions must have same argument types");
        static_assert(std::is_base_of<Object, Sender>::value, "Sender object must be a specialisation of nx::Object");

        Object * sender_obj = nullptr, * receiver_obj = nullptr;
        if constexpr (std::is_base_of<Object, Sender>::value)
            sender_obj = static_cast<Object *>(sender);
        if constexpr (std::is_base_of<Object, Receiver>::value)
            receiver_obj = static_cast<Object *>(receiver);

        if (!sender_obj)
            return false;

        auto func = nx::make_functor(receiver, slot);
        Connection::Type conn_type = static_cast<Connection::Type>(flags & 0x0f);
        uint8_t conn_flags = flags & 0xf0;

        Connection conn (func,
        void_cast(sender), void_cast(signal), void_cast(receiver), void_cast(slot),
        conn_type, conn_flags, receiver_obj != nullptr);

        if (!sender_obj->_getConnectionInfo()->addConnection(std::move(conn)))
            return false;

        if (receiver_obj)
            receiver_obj->_getConnectionInfo()->addSender(std::move(sender_obj));

        return true;
    }

    template <typename Sender, typename Signal, typename ... Args>
    void Object::Emit(Sender* sender, Signal signal, Args&&... args)
    {
        static_assert(std::is_base_of<Object, Sender>::value, "Sender object must be a specialisation of nx::Object");
        Object * sender_obj = static_cast<Object *>(sender);
        auto sender_id = Connection::MakeId(void_cast(sender), void_cast(signal), 0, 0);
        auto connections = sender_obj->_getConnectionInfo()->getConnections(sender_id);
        for (auto & connection : connections)
        {
            connection->transmit(std::forward<Args>(args)...);
        }
    }
#undef void_cast
}

#endif //OBJECT_HPP
