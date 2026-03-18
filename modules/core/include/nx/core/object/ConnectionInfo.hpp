//
// Created by nexie on 08.03.2026.
//

#ifndef NX_CORE_CONNECTION_INFO_HPP
#define NX_CORE_CONNECTION_INFO_HPP

#include <set>
#include <nx/core/object/Connection.hpp>

namespace nx {
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
        std::set<Object *> receivers;

        Object * self { nullptr };

        void _cleanupAfter(const Connection& connection);
        void _removeAll (const Connection& connection);

    public:
        explicit ConnectionInfo (Object * obj);
        ~ConnectionInfo();

        void addSender (Object * sender);
        void removeSender (Object * sender);

        void receiverDestroyed (receiver_id receiver);
        bool addConnection (Connection && connection);
        bool removeConnection (const Connection & connection, bool remove_all = false);

        List<ConnectionPtr> getConnections (sender_id sender) const;

        void cleaup();
    };
}

#endif //NX_CORE_CONNECTION_INFO_HPP
