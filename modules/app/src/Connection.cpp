//
// Created by nexie on 17.11.2025.
//

#include "app/Connection.hpp"

#include <boost/asio/execution/sender.hpp>
#include <sys/socket.h>

#include "app/Object.hpp"
#include "app/Thread.hpp"

using namespace nx;

size_t detail::hash(void* o, void* m)
{
    return std::hash<void *>{}(o) + (std::hash<void *>{}(m) << 1);
}

Connection::id Connection::MakeId(void* sender, void* signal, void* receiver, void* slot)
{
    return detail::hash(sender, signal) + (detail::hash(receiver, slot) << 1);
}

Connection::Connection(FunctorPtr functor, void* sender, void* signal, void* receiver, void* slot, Type type,
    uint8_t flags, bool object_receiver) :
        functor(functor),
        sender(sender),
        signal(signal),
        receiver(receiver),
        slot(slot),
        type(type),
        flags(flags),
        object_receiver(object_receiver)
{
}

bool Connection::isUnique() const
{
    return flags & Flag::Unique;
}

bool Connection::isSingleShot() const
{
    return flags & Flag::SingleShot;
}

bool Connection::isBlocking() const
{
    return flags & Flag::Blocking;
}

void* Connection::getSender() const
{
    return sender;
}

void* Connection::getReceiver() const
{
    return receiver;
}

size_t Connection::getSenderId() const
{
    return Connection::MakeId(sender, signal, nullptr, nullptr);
}

size_t Connection::getId() const
{
    return Connection::MakeId(sender, signal, receiver, slot);
}

void Connection::_transmitImpl(Signal&& s)
{
    // TODO: have to have much more logic than this
    if (type == Direct)
    {
        s.activate();
        return;
    }

    auto dest_thread_id = s.destinationThreadId();

    if (type == Auto && dest_thread_id == Thread::currentId())
    {
        s.activate();
        return;
    }

    auto thread = detail::ThreadInfo::Instance().threadForId(dest_thread_id);
    if (!thread) {
        nxWarning("Destination thread [{}] for signal does not exist!", dest_thread_id);
        return;
    }
    thread->pushSignal(std::move(s), 0);
}


void ConnectionInfo::_cleanupAfter(const Connection& connection)
{
    auto conn_id = connection.getId();
    auto send_id = connection.getSenderId();
    auto recv_id = connection.getReceiver();

    bool conn_list_exists { true };

    if (!connections.contains(conn_id) || connections[conn_id].empty())
        conn_list_exists = false;

    if (!conn_list_exists)
        connections.erase(conn_id);

    if (!conn_list_exists && connections_for_sender.contains(send_id))
    {
        connections_for_sender[send_id].erase(conn_id);
    }

    if (!conn_list_exists && recv_id && connections_for_sender.contains(send_id))
    {
        connections_for_receiver[recv_id].erase(conn_id);
    }
}

void ConnectionInfo::_removeAll(const Connection& connection)
{
    auto id = connection.getId();
    if (connections.contains(id))
    {
        connections[id].clear();
        _cleanupAfter(connection);
    }
}

ConnectionInfo::ConnectionInfo(Object* obj) :
    self (obj)
{

}

nx::ConnectionInfo::~ConnectionInfo()
{
    for (auto & sender : senders)
        sender->_getConnectionInfo()->receiverDestroyed(self);
}

void ConnectionInfo::addSender(Object* sender)
{
    senders.insert(sender);
}

void ConnectionInfo::removeSender(Object* sender)
{
    senders.erase(sender);
}

void ConnectionInfo::receiverDestroyed(receiver_id receiver)
{
    // if (auto connected_senders_it = senders_for_receiver.find(receiver); connected_senders_it != senders_for_receiver.end())
    // {
    //     auto & senders_vec = connected_senders_it->second;
    //     for (auto sender_it = senders_vec.begin(); sender_it != senders_vec.end();)
    //     {
    //         auto & connections_vec = connections[*sender_it];
    //         for (auto conn_it = connections_vec.begin(); conn_it != connections_vec.end();)
    //         {
    //             if ((*conn_it)->getReceiver() == receiver)
    //                 conn_it = connections_vec.erase(conn_it);
    //             else
    //                 ++conn_it;
    //         }
    //
    //         if (connections_vec.empty())
    //             sender_it = senders_vec.erase(sender_it);
    //         else
    //             ++ sender_it;
    //     }
    //     if (senders_vec.empty())
    //         senders_for_receiver.erase(receiver);
    // }
    if (!receiver) return;

    for (auto conn_id: connections_for_receiver[receiver])
    {
        if (!connections.contains(conn_id))
            continue;

        if (connections[conn_id].empty())
        {
            connections.erase(conn_id);
            continue;
        }

        auto conn = *connections[conn_id].front();
        _removeAll(conn);
    }

}

bool ConnectionInfo::addConnection(Connection&& connection)
{
    auto conn_id = connection.getId();
    auto send_id = connection.getSenderId();
    auto recv_id = connection.getReceiver();
    {
        auto & list = connections[conn_id];

        if (!list.empty() &&  connection.isUnique())
            return false;

        list.push_back(std::make_shared<Connection>(std::move(connection)));
    }
    {
        auto & set = connections_for_sender[send_id];
        set.insert(conn_id);
    }
    {
        if (recv_id)
        {
            auto & set = connections_for_receiver[recv_id];
            set.insert(conn_id);
        }
    }
    return true;
}

bool ConnectionInfo::removeConnection(const Connection& connection, bool remove_all)
{
    auto conn_id = connection.getId();

    if (!connections.contains(conn_id))
        return false;

    auto& list = connections[conn_id];
    if (remove_all)
        list.clear();
    else
        if (!list.empty()) list.pop_back();

    _cleanupAfter(connection);
    return true;
}

ConnectionInfo::List<std::shared_ptr<Connection>> ConnectionInfo::getConnections(sender_id sender) const
{
    List<ConnectionPtr> out;
    if (connections.contains(sender))
        return {};

    auto & set = connections_for_sender.at(sender);
    for (auto conn_id: set)
    {
        if (!connections.contains(conn_id))
            continue;

        auto & list = connections.at(conn_id);
        for (auto conn : list)
            out.push_back(conn);
    }

    return out;
}
