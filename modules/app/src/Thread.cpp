//
// Created by nexie on 12.11.2025.
//

#include "app/Thread.hpp"
#include <mutex>
#include <boost/asio.hpp>

#include "app/nxapp.hpp"
#include "app/Loop.hpp"


using namespace nx;

namespace nx::detail
{
    ::nx::ThreadId ThreadInfoInstance::_getNextThreadId() const
    {
        static std::mutex mutex;
        static ThreadId current_id { 1 };

        std::lock_guard<std::mutex> lock(mutex);
        return current_id ++;
    }

    ThreadInfoInstance::~ThreadInfoInstance()
    {
        // nxWarning("Something went wrong while destroying thread info instance");
    }

    ThreadId ThreadInfoInstance::registerThread(Thread* thread)
    {
        if (!thread)
        {
            nxWarning("Passed thread pointer is nullptr");
            return -1;
        }

        if (thread->getId() != g_invalidThreadId)
        {
            nxError("Thread already exists");
            return thread->getId();
        }

        auto id = _getNextThreadId();
        threads_by_id.insert({ id, thread });
        // nxTrace("Registered thread with id: {}", id);
        return id;
    }

    bool ThreadInfoInstance::registerThreadNativeId(NativeThreadId id, Thread * thread)
    {
        if (!thread)
            return false;

        if (thread->getId() == g_invalidThreadId)
        {
            auto nx_id = registerThread(thread);
            if (nx_id == g_invalidThreadId)
                return false;
            thread->id = nx_id;
        }

        if (threads_by_id.count(thread->getId()))
        {
            threads_by_native_id.insert({id, thread});
            return true;
        }

        return false;
    }

    bool ThreadInfoInstance::unregisterThreadNativeId(NativeThreadId id)
    {
        if (threads_by_native_id.count(id))
        {
            threads_by_native_id.erase(id);
            return true;
        }
        return false;
    }

    bool ThreadInfoInstance::unregisterThread(ThreadId threadId)
    {
        if (threads_by_id.count(threadId) == 0)
        {
            nxWarning("Thread with id {} is not registered", threadId);
            return false;
        }

        threads_by_id.erase(threadId);
        return true;
    }

    Thread* ThreadInfoInstance::threadForId(ThreadId id)
    {
        if (threads_by_id.count(id))
            return threads_by_id[id];
        return nullptr;
    }

    Thread* ThreadInfoInstance::threadForNativeId(NativeThreadId id)
    {
        if (threads_by_native_id.count(id))
            return threads_by_native_id[id];
        return nullptr;
    }

    void ThreadInfoInstance::exitAllThreads()
    {
        for (auto [id, thread] : threads_by_id)
            thread->exit(0);
    }

    void ThreadInfoInstance::waitForAllThreadsExit()
    {
        for (auto [id, thread] : threads_by_id)
        {
            thread->waitForExit();
        }
    }
}

Thread::Thread() :
    Object (),
    id (detail::g_invalidThreadId),
    signal_queue(1024)
{
    id = detail::ThreadInfo::Instance().registerThread(this);
    // _reattachToLocalThread();
}

Thread::~Thread()
{
    if (thread and thread->joinable())
    {
        exit( 0 );
        waitForExit();
    }

    detail::ThreadInfo::Instance().unregisterThread(id);
}

Result Thread::start()
{
    thread = std::make_unique<std::thread>(&Thread::_startExecute, this);
    if (thread->joinable())
        return Result::Ok();
    else
        return Result::Err("Failed to start thread");
}

Result Thread::execute()
{
    Loop loop;
    return loop.exec();
}

ThreadId Thread::getId() const
{
    return id;
}

NativeThreadId Thread::getNativeId() const
{
    return native_id;
}


bool Thread::pushSignal(Signal&& signal, int priority)
{
    return signal_queue.pushSignal(std::move(signal), priority);
}

bool Thread::isRunning() const
{
    return running.load(std::memory_order_relaxed);
}

bool Thread::isSleeping() const
{
    return sleeping.load(std::memory_order_relaxed);
}

void Thread::sleep(Duration duration)
{
    this-pushSignal(Signal::Sleep(this, duration), 10);
}

void Thread::sleepUntil(TimePoint t)
{
    auto now = Clock::now();
    if (t > now)
        sleep(now - t);
}

void Thread::exit(int code)
{
    if (running)
    {
        Emit(this, &Thread::aboutToExitSignal);

        pushSignal(Signal::Exit(current_loop, code), 0);
    }
}

void Thread::terminate()
{

}

bool Thread::waitForExit()
{
    if (thread and thread->joinable())
        thread->join();
    thread.reset(nullptr);
    return true;
}

void Thread::quit()
{
    this->exit(0);
}

Thread* Thread::current()
{
    return detail::ThreadInfo::Instance().threadForNativeId(std::this_thread::get_id());
}

Thread* Thread::fromCurrentThread()
{
    auto out = current();

    if (!out)
        out = new LocalThread();

    return out;
}

Loop* Thread::currentLoop()
{
    return current()->current_loop;
}

SignalQueue* Thread::currentQueue()
{
    return current()->queue();
}

Loop* Thread::loop() const
{
    return current_loop;
}

SignalQueue* Thread::queue()
{
    return & signal_queue;
}

void Thread::_sleepImpl(Duration dur)
{
    std::this_thread::sleep_for(dur);
}

Result Thread::_startExecute()
{
    native_id = std::this_thread::get_id();

    try
    {
        if (!detail::ThreadInfo::Instance().registerThreadNativeId(native_id, this))
        {
            return Result::Err("Failed to register thread native id");
        }

    } catch (const std::exception& e) {
        return Result::Err(e.what());
    } catch (...) {
        return Result::Err("Failed to register thread");
    }

    Result res = Result::Ok();
    running.store(true, std::memory_order_relaxed);
    try {
        res = this->execute();
    } catch (const nx::Exception & e) {
        nxError("Exception while executing thread[id:{}]: %s", this->getId(), e.what());
    } catch (const std::exception& e) {
        nxError("Exception while executing thread[id:{}]: %s", this->getId(), e.what());
    } catch ( ... ) {
        nxError("Exception while executing thread[id:{}]", this->getId());
    }
    running.store(false, std::memory_order_relaxed);


    if (!detail::ThreadInfo::Instance().unregisterThreadNativeId(native_id))
    {
        return Result::Err("Failed to unregister thread native id");
    }
    return res;
}

LocalThread::LocalThread() :
    Thread()
{
    if (detail::ThreadInfo::Instance().registerThreadNativeId(std::this_thread::get_id(), this))
    {
        _reattachToThread(this);
        running.store(true, std::memory_order_relaxed);
    }
}

LocalThread::~LocalThread()
{
    detail::ThreadInfo::Instance().unregisterThreadNativeId(std::this_thread::get_id());
}

Result LocalThread::start()
{
    return _startExecute();
}

Result LocalThread::_startExecute()
{
    Result res = Result::Ok();
    try {
        res = this->execute();
    } catch (const nx::Exception & e) {
        nxError("Exception while executing thread[id:{}]: %s", this->getId(), e.what());
    } catch (const std::exception& e) {
        nxError("Exception while executing thread[id:{}]: %s", this->getId(), e.what());
    } catch ( ... ) {
        nxError("Exception while executing thread[id:{}]", this->getId());
    }

    return res;
}
