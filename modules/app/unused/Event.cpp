//
// Created by nexie on 12.11.2025.
//

#include "Event.hpp"
#include "app/nxapp.hpp"
#include "app/Thread.hpp"

using namespace nx;

// void nx::Event::accept()
// {
//     _accepted = true;
// }
//
// nx::SignalEvent::SignalEvent(ThreadId id, InvokerPtr ptr) :
//     Event(Event::Signal),
//     _thread_id(id),
//     _invoker(ptr)
// {
//
// }
//
// void nx::SignalEvent::accept()
// {
//     if (Thread::current()->getId() == _thread_id)
//     {
//         _invoker->invoke();
//         Event::accept();
//     }
//
//     nxError("SignalEvent::accept() - not the target thread!");
// }
//
// nx::TimerEvent::TimerEvent(int timer_id) :
//     Event(Timer)
// {
//
// }
//
// nx::SleepEvent::SleepEvent(Duration dur) :
//     Event(Sleep),
//     duration(dur)
// {
//
// }
//
// nx::SleepEvent::SleepEvent(TimePoint t) :
//     SleepEvent(t - Clock::now())
// {
//
// }

// nx::EventQueue::EventQueue(size_t max_size) :
//     max_size(max_size)
// {
//
// }
//
// nx::EventQueue::~EventQueue()
// {
//
// }
//
// nx::EventQueue::Entry nx::EventQueue::getNext()
// {
//     std::lock_guard<std::mutex> lock(mutex);
//     if (queue.empty())
//         return {};
//     auto const entry = queue.top();
//     queue.pop();
//     return entry;
// }
//
// bool nx::EventQueue::pushEvent(Object* object, Event* event, int priority)
// {
//     std::lock_guard<std::mutex> lock(mutex);
//     if (queue.size() < max_size)
//         queue.push(Entry(priority, object, event));
//     else
//         return false;
//     return true;
// }
//
// bool nx::EventQueue::waitForEvents()
// {
//     // nxTrace("EventLoop::wait_for_events(): waiting...");
//     std::unique_lock<std::mutex> lock(mutex);
//     cv.wait(lock, [&]() { return !queue.empty(); });
//     return !queue.empty();
// }
//
// bool nx::EventQueue::waitForEvents(Duration dur)
// {
//     // nxTrace("EventLoop::wait_for_events(): waiting...");
//     std::unique_lock<std::mutex> lock(mutex);
//     cv.wait_for(lock, dur, [&]() { return !queue.empty(); });
//     return !queue.empty();
// }
//
// size_t nx::EventQueue::count() const
// {
//     // std::lock_guard<const std::mutex> lock(mutex);
//     return queue.size();
// }
//
// bool nx::EventQueue::hasPendingEvents() const
// {
//     // std::lock_guard<const std::mutex> lock(mutex);
//     return !queue.empty();
// }


//
// std::thread::id nx::SignalEvent::targetThread() const
// {
//     return _thread_id;
// }
