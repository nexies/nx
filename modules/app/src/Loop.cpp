//
// Created by nexie on 13.11.2025.
//

#include "nx/app/Loop.hpp"

#include <boost/asio/local/basic_endpoint.hpp>

#include "nx/app/Thread.hpp"
#include "tgbot/net/BoostHttpOnlySslClient.h"

using namespace nx;

Loop::Loop() :
    Object(),
    queue { nullptr },
    underlying_loop(nullptr)
{
    // _getLocalThread()->current_loop = this;
}

Loop::~Loop()
{
    // _getLocalThread()->current_loop = underlying_loop;
}

Result Loop::exec()
{
    // auto thread = _getLocalThread();
    if (!thread())
        return Result::Err("nx::Loop cannot perform exec() not inside an nx::Thread");

    //TODO: pre loop procedures
    _installLoopOntoThread();
    queue = thread()->queue();

    running = true;
    while (running)
    {
        // processEventsFor(Seconds(1));
        processEvents();
    }
    running = false;

    //TODO: post loop procedures
    _uninstallLoopFromThread();

    return Result::Ok({exit_code, "Success"});
}

Result Loop::processEvents()
{
    nxTrace("");
    if (!_waitForSignals())
        return Result::Err("No events");
    while (not interrupt and queue->hasPendingSignals())
    {
        auto entry = queue->getNext();
        _processSingleEntry(entry);
    }
    return Result::Ok();
}

Result Loop::processEventsFor(Duration dur)
{
    nxTrace("");
    auto deadline = Clock::now() + dur;

    if (!_waitForSignalsFor(dur))
        return Result::Err("No events");

    while ((not interrupt) and (Clock::now() < deadline) and (queue->hasPendingSignals()))
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
    running = false;
    return Result::Ok();
}

Result Loop::quit()
{
    if (!running)
        return Result::Err("Loop isn't running");
    thread()->schedule(Signal::Quit(this), 0);
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

void Loop::setWaitDuration(Duration dur)
{
    wait_duration = dur;
}

Duration Loop::waitDuration() const
{
    return wait_duration;
}

bool Loop::_waitForSignals()
{
    if (wait_duration > Duration::zero())
        return _waitForSignalsFor(wait_duration);

    if (!queue) return false;
    if (queue->hasPendingSignals())
        return true;

    sleeping = true;
    queue->waitForSignals();
    sleeping = false;
    return queue->hasPendingSignals();
}

bool Loop::_waitForSignalsFor(Duration dur)
{
    nxTrace("Loop::_waitForSignalsFor");
    if (!queue) return false;
    if (queue->hasPendingSignals())
         return true;

    sleeping = true;
    queue->waitForSignals(dur);
    sleeping = false;
    return queue->hasPendingSignals();
}

bool Loop::_processSingleEntry(SignalQueue::Entry& entry) const
{
    // nxTrace("Loop::_processSingleEntry");

    if (_redirectEntry(entry))
        return false;

    entry.signal.activate();
    return true;
}

bool Loop::_redirectEntry(SignalQueue::Entry & entry) const {
    // nxTrace("Loop::_redirectEntry");
    auto const tid = entry.signal.destinationThreadId();
    // if (tid == detail::g_invalidThreadId)
    //     return false;

    if (threadId() == tid)
        return false;

    auto const thread = detail::ThreadInfo::Instance().threadForId(tid);
    if (!thread) {
        nxCritical("Event's destination thread [{}] does not exist, dropping event :( very bad", tid);
        return true;
    }

    thread->schedule(std::move(entry.signal), entry.priority + 1);
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
    running = false;//, std::memory_order_relaxed);
    interrupt = true;//, std::memory_order_relaxed);
    exit_code = code;
}

void Loop::_interruptImpl()
{
    interrupt = true; //, std::memory_order_relaxed);
}

void Loop::_installLoopOntoThread()
{
    underlying_loop = thread()->current_loop;
    thread()->current_loop = this;
}

void Loop::_uninstallLoopFromThread()
{
    thread()->current_loop = underlying_loop;
    underlying_loop = nullptr;
}
