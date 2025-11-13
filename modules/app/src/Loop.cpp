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
            _processSingleEntry(entry);
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
    while (queue->hasPendingEvents())
    {
        auto entry = queue->getNext();
        _processSingleEntry(entry);
    }
}

bool Loop::_waitForEvents()
{
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
    //TODO:
    if (!entry.event)
    {
        nxError("Entry with no event");
        return false;
    }

    if (entry.receiver)
    {
        auto recv = entry.receiver;
        if (recv->attachedThreadId() != this->attachedThreadId())
        {
            nxWarning("nx::Event in a wrong thread({}). Redirecting with higher priority..", this->attachedThreadId());
            auto dest_thread = detail::ThreadInfo::Instance().threadForId(recv->attachedThreadId());
            if (!dest_thread)
            {
                nxCritical("Destination thread does not exist");
                return false;
            }
            dest_thread->pushEvent(recv, entry.event, entry.priority + 10);
            return true;
        }

        auto res = recv->onEvent(entry.event);
        if (!res)
        {
            nxDebug("Ignored event");
            return true;
        }
        return true;
    }
    else
    {
        auto ev = entry.event;
        if (ev->type() == Event::Signal)
        {
            ev->accept();
            return true;
        } else if (ev->type() == Event::Quit) {
            this->_quitImpl();
        } else if (ev->type() == Event::Exit) {
            this->_exitImpl();
        } else {
            nxWarning("Unhandled event with no receiver object");
            return false;
        }
    }
    return true;
}

void Loop::_quitImpl()
{
    flush();
    running.store(false, std::memory_order_relaxed);
}

void Loop::_exitImpl()
{
    _quitImpl();
    _getLocalThread()->pushEvent(nullptr, new Event(Event::Exit), 0);
}
