//
// Created by nexie on 14.11.2025.
//

#include "../include/nx/core/object/Signal.hpp"

#include "../include/nx/core/Loop.hpp"
#include "../include/nx/core/Object.hpp"
#include "../include/nx/core/Thread.hpp"

using namespace nx;

Signal::Signal(Object * sender, Object* receiver, InvokerPtr invoker) :
    sender(sender),
    receiver(receiver),
    invoker(std::move(invoker))
{

}

Signal::Signal() :
    sender(nullptr),
    receiver(nullptr),
    invoker(nullptr)
{

}

ThreadId Signal::destinationThreadId() const
{
    if (receiver)
        return receiver->threadId();
    return detail::g_invalidThreadId;
}

void Signal::activate() const
{
    Thread::Current()->current_sender = sender;
    if (invoker)
        invoker->invoke();
    Thread::Current()->current_sender = nullptr;
}

Signal Signal::Quit(Object * sender, Loop* loop)
{
    return Signal(sender, loop, &Loop::_quitImpl);
}

Signal Signal::Exit(Object * sender, Loop* loop, int code)
{
    return Signal(sender, loop, &Loop::_exitImpl, code);
}

Signal Signal::Sleep(Object * sender, Thread* thread, int ms)
{
    return Signal(sender, thread, &Thread::_sleepImpl, Milliseconds(ms));
}

Signal Signal::Sleep(Object * sender, Thread* thread, Duration dur)
{
    return Signal(sender, thread, &Thread::_sleepImpl, dur);
}

Signal Signal::SleepUntil(Object * sender, Thread* thread, TimePoint tp)
{
    return Signal(sender, thread, &Thread::_sleepImpl, tp - Clock::now());
}

Signal Signal::Interrupt(Object * sender, Loop* loop)
{
    return Signal(sender, loop, &Loop::_interruptImpl);
}

Signal Signal::NullSignal()
{
    return Signal();
}

void Signal::operator()() const
{
    activate();
}

// SignalQueue::SignalQueue(size_t max_size) :
//     max_size(max_size)
// {
//
// }
//
// SignalQueue::~SignalQueue()
// {
//
// }
//
// SignalQueue::Entry SignalQueue::getNext()
// {
//     std::lock_guard<std::mutex> lock(mutex);
//     if (queue.empty())
//         return Entry(0, Signal::NullSignal());
//     auto entry = queue.top();
//     queue.pop();
//     return entry;
// }
//
// bool SignalQueue::pushSignal(Signal&& signal, int priority)
// {
//     std::lock_guard<std::mutex> lock(mutex);
//     if (queue.size() < max_size)
//     {
//         queue.push(Entry(priority, std::move(signal)));
//         cv.notify_one();
//         return true;
//     }
//     return false;
// }
//
// bool SignalQueue::waitForSignals()
// {
//     std::unique_lock<std::mutex> lock(mutex);
//     cv.wait(lock, [&]() { return !queue.empty(); });
//     return !queue.empty();
// }
//
// bool SignalQueue::waitForSignals(Duration dur)
// {
//     std::unique_lock<std::mutex> lock(mutex);
//     cv.wait_for(lock, dur, [&]() { return !queue.empty(); });
//     return !queue.empty();
// }
//
// size_t SignalQueue::count() const
// {
//     return queue.size();
// }
//
// bool SignalQueue::hasPendingSignals() const
// {
//     return !queue.empty();
// }
