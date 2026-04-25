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
        impl_->install(handle, interest);
    }

    void handle_notifier::setHandle(native_handle_t handle)
    {
        if (impl_->installed())
            impl_->modify(handle, impl_->interest());
        else
            impl_->install(handle, impl_->interest());
    }

    void handle_notifier::setInterest(io_interest interest)
    {
        if (impl_->installed())
            impl_->modify(impl_->handle(), interest);
        else
            impl_->install(impl_->handle(), interest);
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
