//
// Created by nexie on 09.11.2025.
//

#include "../include/nx/core/Object.hpp"

#include <boost/asio/local/basic_endpoint.hpp>

#include "../include/nx/core/Thread.hpp"

using namespace nx;

class Object::Impl
{
    friend class Object;

    Object * obj;
    Thread * local_thread;
    // std::string object_name;
    std::unique_ptr<ConnectionInfo> connection_info;
    std::vector<TimerId> timers;

public:
    Impl(Object * obj) :
        obj(obj),
        connection_info(std::make_unique<ConnectionInfo>(obj)),
        timers({})
    {
        attachToThread(Thread::Current());
    };

    ~Impl()
    {
        connection_info.reset();
        detachFromLocalThread();
    }

    ThreadId localThreadId() const
    {
        if (local_thread)
            return local_thread->getId();
        return detail::g_invalidThreadId;
    }

    Thread* localThread() const
    {
        return local_thread;
    }

    Result detachFromLocalThread()
    {
        // TODO (?) Notify thread about detach?
        local_thread = nullptr;
        return Result::Ok();
    }

    Result attachToThread(Thread* thread)
    {
        detachFromLocalThread();
        local_thread = thread;
        return Result::Ok();
    }

    // Result pushSignal(Signal&& signal, int priority = 0) const
    // {
    //     if (!local_thread)
    //         return Result::Err("pushSignal(): object is not attached to a thread");
    //     local_thread->pushSignal(std::move(signal), priority);
    //     return Result::Ok();
    // }

    Result localThreadSchedule (Signal && signal, int priority) const
    {
        if (!local_thread)
            return Result::Err("pushSignal(): object is not attached to a thread");
        local_thread->schedule(std::move(signal));
        return Result::Ok();
    }

    void cancelTimers() const;

    // std::string objectName() const
    // {
    //     return object_name;
    // }
    //
    // void setObjectName(const std::string& name)
    // {
    //     if (name != object_name)
    //         obj->objectNameChanged(name);
    //     object_name = name;
    // }
};


Object::Object() :
    impl(new Impl(this))
{

}

Object::~Object()
{
    /*emit*/ destroyed();
    delete impl;
}

ThreadId Object::threadId() const
{
    return impl->localThreadId();
}

Thread * Object::thread() const {
    return impl->localThread();
}

Result Object::attachToThread(Thread* thread) const
{
    if (!thread)
        return Result::Err("Target thread pointer is null");

    _reattachToThread(thread);
    return Result::Ok();
}

// std::string Object::objectName() const
// {
//     return impl->objectName();
// }
//
// void Object::setObjectName(const std::string& name)
// {
//     impl->setObjectName(name);
// }

// void Object::_schedule(Signal&& signal, int priority) const
// {
//     auto res = impl->localThreadSchedule(std::forward<Signal>(signal), priority);
// }

// void Object::_generateSignal(Signal&& signal, int priority) const
// {
//     if (auto res = impl->pushSignal(std::move(signal), priority); !res)
//         nxWarning("_generateSignal: {}", res.get_err().str());
// }

Thread* Object::_getLocalThread() const
{
    return impl->localThread();
}

void Object::_reattachToLocalThread() const
{
    _reattachToThread(Thread::Current());
}

void Object::_reattachToThread(Thread* thread) const
{
    impl->attachToThread(thread);
}

ConnectionInfo* Object::_getConnectionInfo() const
{
    return impl->connection_info.get();
}

