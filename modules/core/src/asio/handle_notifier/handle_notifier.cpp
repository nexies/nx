//
// Created by nexie on 23.03.2026.
//

#include <nx/core/asio/handle_notifier.hpp>
#include "handle_notifier_impl.hpp"

namespace nx::asio
{
    HandleNotifier::HandleNotifier(Context& context)
        : impl_ { std::make_shared<Impl>(context) }
    {
    }

    HandleNotifier::HandleNotifier(Context & context, NativeHandle handle, IOInterest interest)
        : impl_ { std::make_shared<Impl>(context) }
    {
        impl_->setHandle(handle);
        impl_->setInterest(interest);
    }

    void HandleNotifier::setHandle(NativeHandle handle)
    {
        impl_->setHandle(handle);
    }

    void HandleNotifier::setInterest(IOInterest interest)
    {
        impl_->setInterest(interest);
    }

    NativeHandle HandleNotifier::handle() const
    {
        return impl_->handle();
    }

    IOInterest HandleNotifier::interest() const
    {
        return impl_->interest();
    }

    void HandleNotifier::cancel()
    {
        impl_->cancel();
    }

    void HandleNotifier::_asyncWaitImpl(HandlerType handler)
    {
        impl_->asyncWait (handler);
    }
}
