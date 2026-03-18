//
// Created by nexie on 09.11.2025.
//

#include "object_impl.p.hpp"
#include "nx/core/Thread.hpp"

using namespace nx;

Object::Object(Object * parent) :
    impl(new Impl(this))
{
    setParent(parent);
}

Object::~Object()
{
    NX_EMIT(destroyed);
    delete impl;
}

ThreadId Object::threadId() const
{
    return impl->getThreadObjectId();
}

Thread * Object::thread() const {
    return impl->getThreadObject();
}

Result Object::attachToThread(Thread* thread) const
{
    if (!thread)
        return Result::Err("Target thread pointer is null");

    _reattachToThread(thread);
    return Result::Ok();
}

void Object::setParent(Object *new_parent) const {
    impl->attachToParentObject(new_parent);
}

Object * Object::parent() const {
    return impl->getParentObject();
}

Object* Object::sender() const
{
    return Thread::CurrentSignalSender();
}

Thread* Object::_getLocalThread() const
{
    return impl->getThreadObject();
}

void Object::_reattachToLocalThread() const
{
    _reattachToThread(Thread::Current());
}

void Object::_reattachToThread(Thread* thread) const
{
    impl->attachToThreadObject(thread);
}

ConnectionInfo* Object::_getConnectionInfo() const
{
    return impl->connection_info_.get();
}

