//
// Created by nexie on 23.03.2026.
//

#include <nx/core/asio/signal_set.hpp>

#include "signal_set_impl.hpp"

namespace nx::asio
{
    SignalSet::SignalSet(Context& ctx) :
        impl_ {std::make_shared<Impl>(ctx)}
    {
    }

    SignalSet::~SignalSet()
    {
        impl_->cancel();
    }

    void SignalSet::add(int signal)
    {
        impl_->add(signal);
    }

    void SignalSet::remove(int signal)
    {
        impl_->remove(signal);
    }

    void SignalSet::clear()
    {
        impl_->clear();
    }

    std::size_t SignalSet::cancel()
    {
        return impl_->cancel();
    }

    void SignalSet::_asyncWaitImpl(HandlerType handler)
    {
        impl_->asyncWait(handler);
    }
}
