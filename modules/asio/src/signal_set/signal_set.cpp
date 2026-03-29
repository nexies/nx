//
// Created by nexie on 23.03.2026.
//

#include <nx/asio/signal_set.hpp>
#include "signal_set_impl.hpp"

namespace nx::asio
{
    signal_set::signal_set(io_context& ctx) :
        impl_ {impl::create_impl(ctx)}
    {
        if (!impl_)
            throw std::runtime_error("signal set not initialized");
    }

    signal_set::~signal_set()
    {
        impl_->cancel();
    }

    void signal_set::add(int signal)
    {
        impl_->add(signal);
    }

    void signal_set::remove(int signal)
    {
        impl_->remove(signal);
    }

    void signal_set::clear()
    {
        impl_->clear();
    }

    std::size_t signal_set::cancel()
    {
        return impl_->cancel();
    }

    void signal_set::_async_wait_impl(handler_type handler)
    {
        impl_->async_wait(handler);
    }
}
