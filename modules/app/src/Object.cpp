//
// Created by nexie on 09.11.2025.
//

#include "app/Object.hpp"

#include <boost/asio/local/basic_endpoint.hpp>

#include "app/Thread.hpp"

using namespace nx;

Object::Object()
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
//
// Result Object::onEvent(Event* ev)
// {
//     if (!ev)
//         return Result::Err("Empty event!");
//
//     switch (ev->type())
//     {
//     case Event::Update:     return onUpdate(ev);
//     case Event::Timer:      return onTimer(static_cast<TimerEvent*>(ev));
//     case Event::Startup:
//     case Event::Quit:
//     case Event::Signal:
//     default:
//         return Result::Err("Unhandled event type");
//     }
// }

// Result Object::notify(Object* o, Event* ev) const
// {
//     if (!o) return Result::Err("Object cannot be nullptr");
//     if (!ev) return Result::Err("Event cannot be nullptr");
//
//     if (o->attachedThreadId() == this->attachedThreadId())
//         o->onEvent(ev);
//     else
//         o->_getLocalThread()->pushSignal(o, ev, 0);
//
//     return Result::Ok();;
// }

// Result Object::onUpdate(Event*)
// {
//     nxDebug("Received update event");
//     return Result::Ok();
// }
//
// Result Object::onTimer(TimerEvent*)
// {
//     return Result::Ok();
// }

// void Object::_generateEvent(Object* o, Event* e, int priority) const
// {
//     local_thread->pushSignal(o, e, priority);
// }

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

