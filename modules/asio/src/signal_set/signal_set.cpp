//
// Created by nexie on 23.03.2026.
//

#include <nx/asio/signal_set.hpp>

#include "signal_set_impl.hpp"

namespace nx::asio
{
    signal_set::signal_set(io_context& ctx) :
        impl_ {std::make_shared<impl>(ctx)}
    {
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

    void signal_set::_asyncWaitImpl(HandlerType handler)
    {
        impl_->asyncWait(handler);
    }
}
