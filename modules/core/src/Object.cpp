//
// Created by nexie on 09.11.2025.
//

#include "nx/core/Object.hpp"

#include <unordered_set>
#include <boost/asio/local/basic_endpoint.hpp>

#include "nx/core/Thread.hpp"

using namespace nx;

class Object::Impl {

    struct ChildrenSet {
        std::unordered_set<Object *> children;

        void add(Object *child) {
            children.insert(child);
        }
        void remove(Object *child) {
            children.erase(child);
        }
    };

    friend class Object;

    Object * obj;
    Object * parent;
    ChildrenSet children;

    Thread * local_thread;
    // std::string object_name;
    std::unique_ptr<ConnectionInfo> connection_info;
    // std::vector<TimerId> timers {};

public:
    Impl(Object * obj) :
        obj(obj),
        connection_info(std::make_unique<ConnectionInfo>(obj)),
        parent(nullptr),
        children({})
    {
        attachToThread(Thread::Current());
    };

    ~Impl()
    {
        connection_info.reset();
        detachFromLocalThread();
    }

    void addChild(Object * child) {
        children.add(child);
    }

    void removeChild(Object * child) {
        children.remove(child);
    }

    bool isTopLevelObject () const {
        return parent == nullptr;
    }

    void detachFromParent() {
        if (parent)
            parent->impl->removeChild(obj);
        parent = nullptr;
    }

    void attachToParent(Object * new_parent) {
        if (new_parent)
            new_parent->impl->addChild(obj);

        parent = new_parent;
    }

    void setParent(Object * new_parent) {
        if (!new_parent)
            new_parent = localThread();

        detachFromParent();
        attachToParent(new_parent);
    }

    Object * getParent() const {
        if (parent == localThread())
            return nullptr;
        return parent;
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


Object::Object(Object * parent) :
    impl(new Impl(this))
{
    impl->setParent(parent);
}

Object::~Object()
{
    NX_EMIT(destroyed);
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

void Object::setParent(Object *new_parent) {
    impl->setParent(new_parent);
}

Object * Object::parent() const {
    return impl->getParent();
}

Object* Object::sender() const
{
    return Thread::CurrentSignalSender();
}

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

