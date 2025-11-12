//
// Created by nexie on 09.11.2025.
//

#include "app/EventLoop.hpp"

#include "tgbot/net/BoostHttpOnlySslClient.h"


void nx::EventLoop::notify_dispatcher(State state)
{
    EventLoopDispatcher::Instance().onEventLoopStateChanged(_exec_thread_id, this, state);
}

nx::EventLoop::EventEntry nx::EventLoop::next_entry()
{
    std::lock_guard<std::mutex> lock(_mutex);
    if (_events.empty())
        return {};
    auto const entry = _events.top();
    _events.pop();
    return entry;
}

nx::Result nx::EventLoop::process_single_event()
{
    try
    {
        auto entry = next_entry();
        if (!entry.event)
        {
            return Result::Err("No events");
        }
        auto ev = entry.event;

        if (ev->type() == Event::Type::Signal)
        {
            auto * signal = static_cast<SignalEvent *>(ev);
            if (_exec_thread_id == signal->targetThread())
            {
                signal->accept();
                return Result::Ok("Signal processed");
            }
            auto target = EventLoopDispatcher::Instance().getEventLoop(signal->targetThread());
            if (!target)
                return Result::Err("Signal located in wrong event loop and can't be redirected (target event loop does not exist)");

            target->addEvent(signal, entry.priority);
            return Result::Ok("Signal redirected into it's target thread");
        }

        // TODO: Event dispatcher here ?
        return Result::Err("Not implemented");
    }
    catch ( const nx::Exception & ex ) { return Result::Err(ex.what()); }
    catch ( const std::exception & ex ) { return Result::Err(ex.what()); }
    catch ( ... ) { return Result::Err("Unknown exception"); }
}

void nx::EventLoop::wait_for_events()
{
    nxTrace("EventLoop::wait_for_events(): waiting...");
    std::unique_lock<std::mutex> lock(_mutex);
    _has_events_cv.wait_for(lock, Seconds(1), [&]() { return !_events.empty(); });
}

nx::EventLoop::EventLoop() :
    _exec_thread_id(std::this_thread::get_id())
{
    notify_dispatcher(State::Created);
}

nx::EventLoop::~EventLoop()
{
    notify_dispatcher(State::Destroyed);
}

void nx::EventLoop::addEvent(Event* ev, int pr)
{
    if (ev)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _events.push({pr, ev});
    }
}

nx::Result nx::EventLoop::processEvents()
{
    while (true)
    {
        auto res = process_single_event();
        if (!res) return res;
    }
    return Result::Ok();
}

nx::Result nx::EventLoop::processEventsFor(Duration dur)
{
    auto cur = Clock::now();
    while (Clock::now() < cur + dur)
    {
        auto res = process_single_event();
        if (!res) return res;
    }
    return Result::Ok();
}

nx::Result nx::EventLoop::processEventsUntil(TimerPoint tp)
{
    return processEventsFor(tp - Clock::now());
}

nx::Result nx::EventLoop::exec()
{
    _exec_thread_id = std::this_thread::get_id();
    notify_dispatcher(State::Started);

    _should_exit = false;
    while (!_should_exit)
    {
        auto res = processEvents();
        wait_for_events();
    }

    notify_dispatcher(State::Exited);
    return Result::Ok("Loop Exited");
}

void nx::EventLoop::exit()
{
    _should_exit = true;
    notify_dispatcher(State::Exiting);
}


void nx::EventLoopDispatcherInstance::onEventLoopStateChanged(id i, EventLoop* l, EventLoop::State state)
{
    switch (state)
    {
    case EventLoop::State::Created:
        return addEventLoop(i, l);
    case EventLoop::State::Destroyed:
        return removeEventLoop(i);
    case EventLoop::State::Starting:
    case EventLoop::State::Started:
    case EventLoop::State::Paused:
    case EventLoop::State::Exiting:
    case EventLoop::State::Exited:
        nxDebug("Unhandled EventLoop state");
    }
}

void nx::EventLoopDispatcherInstance::addMainEventLoop(id p_id, EventLoop* l)
{
    addEventLoop(p_id, l);
    _main = l;
}

void nx::EventLoopDispatcherInstance::addEventLoop(id p_id, EventLoop* p_loop)
{
    std::lock_guard<std::mutex> lock(_m);
    _d[p_id] = p_loop;
}

void nx::EventLoopDispatcherInstance::removeEventLoop(id p_id)
{
    std::lock_guard<std::mutex> lock(_m);
    _d.erase(p_id);
}

nx::EventLoop* nx::EventLoopDispatcherInstance::getEventLoop(id p_id)
{
    if (auto const it = _d.find(p_id); it != _d.end())
        return it->second;
    return nullptr;
}

void nx::EventLoopDispatcherInstance::setLoopState(id, EventLoop::State)
{

}

nx::EventLoop* nx::EventLoopDispatcherInstance::mainEventLoop()
{
    return _main;
}
