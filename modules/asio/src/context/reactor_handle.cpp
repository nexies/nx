//
// Created by nexie on 24.03.2026.
//

#include <nx/asio/context/reactor_handle.hpp>
#include <nx/common/types/errors/codes.hpp>

#include <utility>
#include "context_impl.hpp"

namespace nx::asio
{
    reactor_handle::reactor_handle(io_context & ctx)
        : ctx_      { ctx }
        , interest_ { io_interest::none }
        , handle_   { g_null_handle }
        , installed_{ false }
    {}

    reactor_handle::~reactor_handle()
    {
        uninstall();
    }

    void reactor_handle::install(native_handle_t handle, io_interest interest)
    {
        if (installed_)
            throw nx::err::invalid_state("reactor_handle::install(): already installed");

        ctx_.impl_->register_reactor_handle(handle, this, interest);
        handle_    = handle;
        interest_  = interest;
        installed_ = true;
    }

    void reactor_handle::modify(native_handle_t handle, io_interest interest)
    {
        if (!installed_)
            throw nx::err::invalid_state("reactor_handle::modify(): not installed");

        ctx_.impl_->modify_reactor_handle(handle, this, interest);
        handle_   = handle;
        interest_ = interest;
    }

    void reactor_handle::uninstall()
    {
        if (!installed_)
            return;
        ctx_.impl_->unregister_reactor_handle(handle_, this, interest_);
        handle_    = g_null_handle;
        interest_  = io_interest::none;
        installed_ = false;
    }

    void reactor_handle::set_read_handler(handler_t h)
    {
        read_handler_ = std::move(h);
    }

    void reactor_handle::set_write_handler(handler_t h)
    {
        write_handler_ = std::move(h);
    }

    void reactor_handle::on_event(backend_event & event)
    {
        // One-shot: exchange handler with nullptr before calling so that
        // re-arming inside the handler works and exceptions leave a clean state.
        if ((event.events & io_event::read) != io_event::none) {
            if (auto h = std::exchange(read_handler_, nullptr))
                h(event);
        }
        if ((event.events & io_event::write) != io_event::none) {
            if (auto h = std::exchange(write_handler_, nullptr))
                h(event);
        }
        // error/hangup: try both handlers so either side can react
        if ((event.events & (io_event::error | io_event::hangup)) != io_event::none) {
            if (auto h = std::exchange(read_handler_,  nullptr)) h(event);
            if (auto h = std::exchange(write_handler_, nullptr)) h(event);
        }
    }

    io_interest reactor_handle::interest() const  { return interest_; }
    native_handle_t reactor_handle::handle()  const  { return handle_;   }
    bool reactor_handle::installed() const  { return installed_; }
    io_context & reactor_handle::ctx() const  { return ctx_;      }
}
