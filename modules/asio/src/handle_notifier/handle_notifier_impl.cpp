//
// Created by nexie on 23.03.2026.
//

#include "handle_notifier_impl.hpp"

#include "../context/context_impl.hpp"

namespace nx::asio
{
    handle_notifier::impl::impl(io_context& ctx)
        : reactor_handle(ctx)
        , handler_ { nullptr }
    {

    }

    void handle_notifier::impl::cancel()
    {
        handler_ = nullptr;
        uninstall();
    }

    void handle_notifier::impl::async_wait(HandlerType callback)
    {
        handler_ = std::move(callback);
    }

    void handle_notifier::impl::react(io_event event)
    {
        if (event != io_event::none)
            handle_event(event);
    }

    void handle_notifier::impl::handle_event(io_event ev)
    {
        if (!handler_)
            return;

        ctx().post([h = std::move(handler_), fd = handle(), ev = ev]
        {
            h(fd, ev);
        });

        handler_ = nullptr;
    }
}
