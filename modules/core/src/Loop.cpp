//
// Created by nexie on 13.11.2025.
//

// #include <boost/asio/local/basic_endpoint.hpp>

#include <nx/core/detail/logger_defs.hpp>
#include <nx/core/Loop.hpp>
#include <nx/core/Thread.hpp>

using namespace nx;

Loop::Loop() :
    Object()
{ }

Loop::~Loop() { }

Result Loop::exec()
{
    if (!thread())
        return Result::Err({10, "nx::Loop cannot perform exec() not inside an nx::Thread"});

    if (thread() != Thread::Current())
    {
        nxWarning("nx::Loop calling exec outside the thread the Loop was created in. Will reattach to Thread::Current()");
        _reattachToLocalThread();
    }

    //TODO: pre loop procedures
    _installLoopOntoThread();
    thread()->context_locker.lock();

    running = true;
    thread()->context().run();
    running = false;

    //TODO: post loop procedures
    _uninstallLoopFromThread();

    return Result::Ok({exit_code, "Success"});
}

Result Loop::processEvents()
{
    nxDevTrace("Loop::processEvents()");
    size_t count = 0;
    try
    {
        count = thread()->context().poll();
    } catch ( boost::system::error_code & e ) {
        return Result::Err(fmt::format ("Process events error: {}", e.what()));
    } catch ( std::exception & e ) {
        return Result::Err(fmt::format ("Process events error: {}", e.what()));;
    } catch ( ... ) {
        return Result::Err("Process events error");
    }

    return Result::Ok(fmt::format("Processed events: {}", count));
}

Result Loop::processEventsFor(Duration dur)
{
    nxTrace("Loop::processEventsFor()");
    size_t count = 0;
    try
    {
        count = thread()->context().runFor(dur);
    } catch ( boost::system::error_code & e ) {
        return Result::Err(fmt::format ("Process events error: {}", e.what()));
    } catch ( std::exception & e ) {
        return Result::Err(fmt::format ("Process events error: {}", e.what()));;
    } catch ( ... ) {
        return Result::Err("Process events error");
    }

    return Result::Ok(fmt::format("Processed events: {}", count));
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
    nxTrace("Loop::quit");
    if (!running)
        return Result::Err("Loop isn't running");
    return Result::Ok();
}

bool Loop::isRunning() const {
    return running;
}

bool Loop::isSleeping() const {
    return sleeping;
}

void Loop::flush()
{
    thread()->context().poll();
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
    // if (wait_duration > Duration::zero())
        // return _waitForSignalsFor(wait_duration);

    // if (!queue) return false;
    // if (queue->hasPendingSignals())
        // return true;

    // sleeping = true;
    // queue->waitForSignals();
    // sleeping = false;
    // return queue->hasPendingSignals();
    return false;
}

bool Loop::_waitForSignalsFor(Duration dur)
{
    // nxTrace("Loop::_waitForSignalsFor");
    // if (!queue) return false;
    // if (queue->hasPendingSignals())
         // return true;

    // sleeping = true;
    // queue->waitForSignals(dur);
    // sleeping = false;
    // return queue->hasPendingSignals();
    return false;
}

void Loop::_quitImpl()
{
    _exitImpl(0);
}

void Loop::_exitImpl(int code)
{
    nxTrace("Loop::_exitImpl");
    flush();
    thread()->context_locker.unlock();
    thread()->context().stop();
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
    thread()->loops.push(this);
}

void Loop::_uninstallLoopFromThread()
{
    thread()->loops.pop();
}
