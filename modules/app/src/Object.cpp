//
// Created by nexie on 09.11.2025.
//

#include "app/Object.hpp"

#include <boost/asio/local/basic_endpoint.hpp>

#include "app/Thread.hpp"

using namespace nx;

Object::Object() :
    connection_info(std::make_unique<ConnectionInfo>(this))
{
    _reattachToLocalThread();
}

Object::~Object()
{

}

ThreadId Object::attachedThreadId() const
{
    if (auto thread = _getLocalThread(); thread)
        return thread->getId();
    return detail::g_invalidThreadId;
}

Result Object::attachToThread(Thread* thread)
{
    if (!thread)
        return Result::Err("Target thread pointer is null");

    _reattachToThread(thread);
    return Result::Ok();
}

void Object::_generateSignal(Signal&& signal, int priority) const
{
    local_thread->pushSignal(std::move(signal), priority);
}

Thread* Object::_getLocalThread() const
{
    return local_thread;
}

void Object::_reattachToLocalThread()
{
    _reattachToThread(Thread::current());
}

void Object::_reattachToThread(Thread* thread)
{
    local_thread = thread;
}

ConnectionInfo* Object::_getConnectionInfo() const
{
    return connection_info.get();
}

