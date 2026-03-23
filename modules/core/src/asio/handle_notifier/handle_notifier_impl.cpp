//
// Created by nexie on 23.03.2026.
//

#include "handle_notifier_impl.hpp"

#include "../context/context_impl.hpp"

namespace nx::asio
{
    HandleNotifier::Impl::Impl(Context& ctx): ctx_ { ctx }
                                              , fd_ { g_nullHandle }
                                              , interest_ { IOInterest::None }
                                              , is_waiting_ { false }
                                              , registered_ { false }
    {

    }

    HandleNotifier::Impl::~Impl()
    {
        unreg();
    }

    void HandleNotifier::Impl::setHandle(NativeHandle handle)
    {
        unreg();
        fd_ = handle;
        reg();
    }

    void HandleNotifier::Impl::setInterest(IOInterest interest)
    {
        unreg();
        interest_ = interest;
        reg();
    }

    NativeHandle HandleNotifier::Impl::handle() const
    {
        return fd_;
    }

    IOInterest HandleNotifier::Impl::interest() const
    {
        return interest_;
    }

    void HandleNotifier::Impl::cancel()
    {
        if (!is_waiting_)
            return;

        is_waiting_ = false;
        handler_ = nullptr;
    }

    void HandleNotifier::Impl::asyncWait(HandlerType callback)
    {
        if (is_waiting_)
            throw std::logic_error {"HandleNotifier already has pending asyncWait"};

        if (!registered_)
            throw std::logic_error {"HandlerNotifier is missing NativeHandle or IOInterest"};

        is_waiting_ = true;
        handler_ = callback;
    }

    void HandleNotifier::Impl::react(IOEvent event)
    {
        if (event != IOEvent::None)
            handle_event(event);
    }

    void HandleNotifier::Impl::reg()
    {
        if (registered_)
            return;

        if (fd_ == g_nullHandle)
            return;

        if (interest_ == IOInterest::None)
            return;

        ctx_.impl_->registerReactorHandle(fd_, this, interest_);
        registered_ = true;
    }


    void HandleNotifier::Impl::unreg()
    {
        if (!registered_)
            return;

        ctx_.impl_->unregisterReactorHandle(fd_);
        registered_ = false;
    }

    void HandleNotifier::Impl::handle_event(IOEvent ev)
    {
        if (!is_waiting_)
            return;

        is_waiting_ = false;

        ctx_.post([h = std::move(handler_), fd = fd_, ev = ev]
        {
            h(fd, ev);
        });

        handler_ = nullptr;
    }
}
