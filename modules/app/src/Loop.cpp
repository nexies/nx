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
    queue { nullptr },
    underlying_loop(_getLocalThread()->current_loop)
{
    _getLocalThread()->current_loop = this;
}

Loop::~Loop()
{
    _getLocalThread()->current_loop = underlying_loop;
}

Result Loop::exec()
{
    auto thread = _getLocalThread();
    if (!thread)
        return Result::Err("nx::Loop cannot perform exec() not inside an nx::Thread");

    queue = thread->queue();

    //TODO: pre loop procedures
    running.store(true);
    while (running)
    {
        // processEventsFor(Seconds(1));
        processEvents();
    }
    running.store(false);

    //TODO: post loop procedures

    return Result::Ok({exit_code, "Success"});
}

Result Loop::processEvents()
{
    if (!_waitForSignals())
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

    if (!_waitForSignalsFor(dur))
        return Result::Err("No events");

    while ((not interrupt) and (Clock::now() < deadline))
    {
        auto entry = queue->getNext();
       _processSingleEntry(entry);
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
    _generateSignal(Signal::Quit(this), 0);
    return Result::Ok();
}

void Loop::flush()
{
    nxTrace("Loop::flush()");
    while (queue->hasPendingSignals())
    {
        auto entry = queue->getNext();
        _processSingleEntry(entry);
    }
}

bool Loop::_waitForSignals()
{
    nxTrace("Loop::_waitForSignals");
    if (!queue) return false;
    if (queue->hasPendingSignals())
        return true;

    sleeping.store(true);
    queue->waitForSignals();
    sleeping.store(false);
    return queue->hasPendingSignals();
}

bool Loop::_waitForSignalsFor(Duration dur)
{
    nxTrace("Loop::_waitForSignalsFor");
    if (!queue) return false;
    if (queue->hasPendingSignals())
         return true;

    sleeping.store(true);
    queue->waitForSignals(dur);
    sleeping.store(false);
    return queue->hasPendingSignals();
}

bool Loop::_processSingleEntry(SignalQueue::Entry& entry) const
{
    nxTrace("Loop::_processSingleEntry");

    if (_redirectEntry(entry))
        return false;

    entry.signal.activate();
    return true;
}

bool Loop::_redirectEntry(SignalQueue::Entry & entry) const {
    nxTrace("Loop::_redirectEntry");
    auto const tid = entry.signal.destinationThreadId();
    if (attachedThreadId() == tid)
        return false;

    auto const thread = detail::ThreadInfo::Instance().threadForId(tid);
    if (!thread) {
        nxCritical("Event's destination thread does not exist, dropping event :( very bad");
        return true;
    }

    thread->pushSignal(std::move(entry.signal), entry.priority + 1);
    return true;
}

void Loop::_quitImpl()
{
    _exitImpl(0);
}

void Loop::_exitImpl(int code)
{
    nxTrace("Loop::_exitImpl");
    flush();
    running.store(false, std::memory_order_relaxed);
    interrupt.store(true, std::memory_order_relaxed);
    exit_code = code;
}

void Loop::_interruptImpl()
{
    interrupt.store(true, std::memory_order_relaxed);
}
