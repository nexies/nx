//
// Created by nexie on 09.11.2025.
//

#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <list>
#include <set>

// #include "Object.hpp"
#include "app/Signal.hpp"
#include "core/store/DataPool.h"


namespace nx {

    class Object;
    namespace detail
    {
        size_t hash ( void * o, void * m );
    }

    class Connection
    {
    public:
        using id = size_t;
        static id MakeId ( void * sender, void * signal, void * receiver, void * slot );

        enum Type : uint8_t
        {
            Auto            = 0x00,
            Direct          = 0x01,
            Queued          = 0x02,
        };

        enum Flag : uint8_t
        {
            NoFlags         = 0x00,
            Unique          = 0x10,
            SingleShot      = 0x20,
            Blocking        = 0x40,
        };

        Connection (FunctorPtr functor,
            void * sender, void * signal,
            void * receiver, void * slot,
            Type type, uint8_t flags,
            bool object_receiver = false);

        template<typename... Args>
        void transmit (Args&&...);

        [[nodiscard]] bool isUnique () const;
        [[nodiscard]] bool isSingleShot () const;
        [[nodiscard]] bool isBlocking () const;
        [[nodiscard]] void * getSender () const;
        [[nodiscard]] void * getReceiver () const;
        [[nodiscard]] size_t getSenderId () const;
        [[nodiscard]] size_t getId () const;

    protected:
        void _transmitImpl (Signal && s);

        FunctorPtr functor;
        void * sender;
        void * signal;
        void * receiver;
        void * slot;

        bool object_receiver;
        Type type;
        uint8_t flags;
    };

    template <typename... Args>
    void Connection::transmit (Args&&... args)
    {
        if (object_receiver)
        {
            auto r = static_cast <Object *> (receiver);
            return _transmitImpl (Signal(r, functor, std::forward<Args>(args)...));
        }
        return _transmitImpl (Signal(functor, std::forward<Args>(args)...));
    }

    using ConnectionPtr = std::shared_ptr<Connection>;
    using ConnectionRef = std::weak_ptr<Connection>;

    class ConnectionInfo
    {
        /// Индексы:

        ///     - by sender + signal + receiver + slot (уникальный в рамках разных соединений) -
        using connection_id = Connection::id;
        ///     - by sender object + signal (не уникальный) - для быстрого получения списка соединений при вызове сигнала
        using sender_id = size_t;
        ///     - by receiver object (не уникальный) - для очистки соединений при удалении объекта
        using receiver_id = void*;

        template<typename T>
        using List = std::vector<T>;

        template<typename T>
        using Set = std::set<T>;

        std::unordered_map<connection_id, List<ConnectionPtr>> connections;
        std::unordered_map<sender_id, Set<connection_id>> connections_for_sender;
        std::unordered_map<receiver_id, Set<connection_id>> connections_for_receiver;

        std::set<Object *> senders;

        Object * self { nullptr };

        void _cleanupAfter(const Connection& connection);
        void _removeAll (const Connection& connection);

    public:
        ConnectionInfo (Object * obj);
        ~ConnectionInfo();

        void addSender (Object * sender);
        void removeSender (Object * sender);

        void receiverDestroyed (receiver_id receiver);
        bool addConnection (Connection && connection);
        bool removeConnection (const Connection & connection, bool remove_all = false);

        List<ConnectionPtr> getConnections (sender_id sender) const;
    };


}
#endif //CONNECTION_HPP
