//
// Created by nexie on 09.11.2025.
//

#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <nx/core.hpp>
#include <nx/core/Signal.hpp>
#include <nx/core/Connection.hpp>

namespace nx {

    class Thread;
    class ConnectionInfo;

    class Object {
        friend class ConnectionInfo;
        template<typename Sender, typename Signal, typename Receiver, typename Slot>
        friend bool connect (Sender * sender, Signal && signal, Receiver * receiver, Slot && slot, uint8_t flags);

        template<typename Sender, typename Signal, typename Receiver, typename Slot>
        friend bool disconnect (Sender * sender, Signal && signal, Receiver * receiver, Slot && slot, bool disconnect_all);

        template<typename Sender, typename Signal, typename ... Args>
        friend void emit (Sender * sender, Signal signal, Args&&...);

    public:
        Object ();
        virtual ~Object ();

        [[nodiscard]]
        ThreadId threadId () const;
        [[nodiscard]]
        Thread * thread () const;
        [[nodiscard]]
        Result attachToThread (Thread *) const;

        template<typename EventType>
        Result event (EventType &);

        NX_PROPERTY(TYPE std::string, NAME objectName, READ objectName, WRITE setObjectName, NOTIFY objectNameChanged);
        NX_SIGNAL(destroyed)

    protected:
        // void _schedule(Signal && signal, int priority = 0) const;
        // void _generateSignal (Signal && signal, int priority = 0) const;

        Thread * _getLocalThread () const;
        void _reattachToLocalThread () const;
        void _reattachToThread (Thread *) const;
        ConnectionInfo * _getConnectionInfo () const;
    private:

        class Impl;
        Impl * impl;
    };

    template <typename EventType>
    Result Object::event(EventType&)
    {
        return Result::Err("Not supported event type");
    }

#define void_cast(val) reinterpret_cast<void *>(val)

    template <typename Sender, typename Signal, typename Receiver, typename Slot>
    bool connect(Sender* sender, Signal&& signal, Receiver* receiver, Slot&& slot, uint8_t flags)
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

    template <typename Sender, typename Signal, typename Receiver, typename Slot>
    bool disconnect(Sender* sender, Signal&& signal, Receiver* receiver, Slot&& slot, bool disconnect_all)
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
        // Connection::Type conn_type = static_cast<Connection::Type>(flags & 0x0f);
        // uint8_t conn_flags = flags & 0xf0;

        Connection conn (func,
        void_cast(sender), void_cast(signal), void_cast(receiver), void_cast(slot),
        Connection::Auto, 0, receiver_obj != nullptr);

        if (!sender_obj->_getConnectionInfo()->removeConnection(conn, disconnect_all))
            return false;

        if (receiver_obj)
            receiver_obj->_getConnectionInfo()->removeSender(std::move(sender_obj));

        return true;
    }

    template <typename Sender, typename Signal, typename Receiver, typename Slot>
    bool connect(Sender* sender, Signal&& signal, Receiver* receiver, Slot&& slot)
    {
        return connect(sender, signal, receiver, slot, Connection::Auto);
    }

    template <typename Sender, typename Signal, typename Receiver, typename Slot>
    bool connect_queued(Sender* sender, Signal&& signal, Receiver* receiver, Slot&& slot)
    {
        return connect(sender, signal, receiver, slot, Connection::Queued);
    }

    template <typename Sender, typename Signal, typename Receiver, typename Slot>
    bool connect_direct(Sender* sender, Signal&& signal, Receiver* receiver, Slot&& slot)
    {
        return connect(sender, signal, receiver, slot, Connection::Direct);
    }

    template <typename Sender, typename Signal, typename Receiver, typename Slot>
    bool disconnect(Sender* sender, Signal&& signal, Receiver* receiver, Slot&& slot)
    {
        return disconnect(sender, signal, receiver, slot, false);
    }

    template <typename Sender, typename Signal, typename Receiver, typename Slot>
    bool disconnect_all(Sender* sender, Signal&& signal, Receiver* receiver, Slot&& slot)
    {
        return disconnect(sender, signal, receiver, slot, true);
    }

    template <typename Sender, typename Signal, typename ... Args>
    void emit(Sender* sender, Signal signal, Args&&... args)
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
