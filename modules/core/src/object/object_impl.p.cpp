//
// Created by nexie on 17.03.2026.
//

#include "object_impl.p.hpp"

// #include <boost/asio/local/basic_endpoint.hpp>
#include <nx/core/Thread.hpp>

Object::Impl::Impl(Object* obj) noexcept :
    obj_ { obj },
    connection_info_ { std::make_unique<ConnectionInfo>(obj) },
    children_ { },
    parent_ { nullptr }
{
    attachToThreadObject(Thread::Current());
}

Object::Impl::~Impl()
{
    // connection_info_->cleaup();
    connection_info_.reset();
}

void Object::Impl::addChildObject(Object* child)
{
    children_.push_back(child);
}

void Object::Impl::removeChildObject(Object* child)
{
    // auto it = std::remove(children_.begin(), children_.end(), child);
    for (auto it = children_.begin(); it != children_.end(); ++it)
        if (*it == child)
        {
            children_.erase(it);
        }
}

bool Object::Impl::isTopLevelObject() const
{
    return (!parent_);
}

void Object::Impl::detachFromParentObject()
{
    if (parent_)
        parent_->impl->removeChildObject(obj_);

    parent_ = nullptr;
}

void Object::Impl::attachToParentObject(Object* parent)
{
    if (local_thread_ && isTopLevelObject())
        local_thread_->impl->removeChildObject(obj_);

    detachFromParentObject();

    parent_ = parent;

    if (parent_)
        parent_->impl->addChildObject(obj_);
    else if (local_thread_)
        local_thread_->impl->addChildObject(obj_);
}

Object* Object::Impl::getParentObject() const
{
    return parent_;
}

ThreadId Object::Impl::getThreadObjectId() const
{
    if (local_thread_)
        return local_thread_->getId();

    return nx::detail::g_invalidThreadId;
}

Thread* Object::Impl::getThreadObject() const
{
    return local_thread_;
}

void Object::Impl::detachFromThreadObject()
{
    if (!local_thread_)
        return;

    if (isTopLevelObject())
        local_thread_->impl->removeChildObject(obj_);

    local_thread_ = nullptr;
}

void Object::Impl::attachToThreadObject(Thread* thread) noexcept
{
    detachFromThreadObject();

    local_thread_ = thread;
    if (local_thread_ && isTopLevelObject())
        local_thread_->impl->addChildObject(obj_);
}

// ThreadId Object::Impl::localThreadId() const
// {
//     if (!local_thread_)
//         return nx::detail::g_invalidThreadId;
//     return local_thread_->threadId();
// }
//
// Result Object::Impl::attachToThread(Thread* thread) noexcept
// {
//     if (!thread)
//     {
//
//     }
// }
