//
// Created by nexie on 13.11.2025.
//

#include "app/Loop.hpp"

#include <boost/asio/local/basic_endpoint.hpp>

#include "app/Thread.hpp"
#include "tgbot/net/BoostHttpOnlySslClient.h"

using namespace nx;

Loop::Loop() :
    Object(),
    queue { nullptr }
{

}

Result Loop::exec()
{
    auto thread = _getLocalThread();
    if (!thread)
        return Result::Err("nx::Loop cannot perform exec() not inside an nx::Thread");

    queue = thread->getQueue();

    //TODO: pre loop procedures
    running.store(true);
    while (running)
    {
        processEventsFor(Seconds(1));
    }
    running.store(false);

    //TODO: post loop procedures

    return Result::Ok();
}

Result Loop::processEvents()
{
    if (!_waitForEvents())
        return Result::Err("No events");
    while (not interrupt)
    {
        auto entry = queue->getNext();
        _processSingleEntry(entry);
    }
    return Result::Ok();
}

Result Loop::processEventsFor(Duration dur)
{
    auto deadline = Clock::now() + dur;

    if (!_waitForEventsFor(dur))
        return Result::Err("No events");

    while ((not interrupt) and (Clock::now() < deadline))
    {
        auto entry = queue->getNext();
        if (entry.event)
        {
            if (_processSingleEntry(entry))
                delete entry.event;
        }
        else
            break;
    }

    return Result::Ok();
}

Result Loop::processEventsUntil(TimePoint t)
{
    if (t > Clock::now())
        return processEventsFor(t - Clock::now());
    return Result::Err("Invalid time point");
}

Result Loop::exit()
{
    nxDebug("Loop will exit");
    running.store(false);
    return Result::Ok();
}

Result Loop::quit()
{
    if (!running)
        return Result::Err("Loop isn't running");
    _generateEvent(this, new Event(Event::Quit), 0);
    return Result::Ok();
}

void Loop::flush()
{
    nxTrace("Loop::flush()");
    while (queue->hasPendingEvents())
    {
        auto entry = queue->getNext();
        _processSingleEntry(entry);
    }
}

bool Loop::_waitForEvents()
{
    nxTrace("Loop::_waitForEvents");
    if (!queue) return false;
    if (queue->hasPendingEvents())
        return true;

    sleeping.store(true);
    queue->waitForEvents();
    sleeping.store(false);
    return queue->hasPendingEvents();
}

bool Loop::_waitForEventsFor(Duration dur)
{
    nxTrace("Loop::_waitForEvents");
    if (!queue) return false;
    if (queue->hasPendingEvents())
         return true;

    sleeping.store(true);
    queue->waitForEvents(dur);
    sleeping.store(false);
    return queue->hasPendingEvents();
}

bool Loop::_processSingleEntry(EventQueue::Entry& entry)
{
    nxTrace("Loop::_processSingleEntry");
    //TODO:
    if (!entry.event)
    {
        nxError("Entry with no event");
        return false;
    }

    if (_redirectEntry(entry))
        return false;

    if (entry.event->type() == Event::Exit) {
        this->_exitImpl();
        return true;
    }

    if (entry.event->type() == Event::Signal) {
        entry.event->accept();
        return true;
    }

    if (entry.receiver)
    {
        entry.receiver->onEvent(entry.event);
        return true;
    }

    switch (entry.event->type()) {
        default:
            nxError("Unknown event type");
            return true;
    }
}

bool Loop::_redirectEntry(EventQueue::Entry &entry) {
    nxTrace("Loop::_redirectEntry");
    auto const recv = entry.receiver;
    if (!recv)
        return false;

    auto const tid = recv->attachedThreadId();
    if (attachedThreadId() == tid)
        return false;

    auto const thread = detail::ThreadInfo::Instance().threadForId(tid);
    if (!thread) {
        nxCritical("Event's destination thread does not exist, dropping event :( very bad");
        delete entry.event;
        return true;
    }

    thread->pushEvent(recv, entry.event, entry.priority + 1);
    return true;
}

void Loop::_quitImpl()
{
    flush();
    running.store(false, std::memory_order_relaxed);
    interrupt.store(true, std::memory_order_relaxed);
}

void Loop::_exitImpl()
{
    nxTrace("Loop::_exitImpl");
    _quitImpl();
    _getLocalThread()->pushEvent(nullptr, new Event(Event::Exit), 0);
}
