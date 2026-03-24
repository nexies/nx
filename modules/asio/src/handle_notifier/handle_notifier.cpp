//
// Created by nexie on 23.03.2026.
//

#include <nx/asio/handle_notifier.hpp>
#include "handle_notifier_impl.hpp"

namespace nx::asio
{
    handle_notifier::handle_notifier(io_context& context)
        : impl_ { std::make_shared<impl>(context) }
    {
    }

    handle_notifier::handle_notifier(io_context & context, native_handle_t handle, io_interest interest)
        : impl_ { std::make_shared<impl>(context) }
    {
        // impl_->setHandle(handle);
        // impl_->setInterest(interest);
    }

    void handle_notifier::setHandle(native_handle_t handle)
    {
        // impl_->setHandle(handle);
    }

    void handle_notifier::setInterest(io_interest interest)
    {
        // impl_->setInterest(interest);
    }

    native_handle_t handle_notifier::handle() const
    {
        return impl_->handle();
    }

    io_interest handle_notifier::interest() const
    {
        return impl_->interest();
    }

    void handle_notifier::cancel()
    {
        impl_->uninstall();
    }

    void handle_notifier::_async_wait_impl(HandlerType handler)
    {
        impl_->async_wait (handler);
    }
}
