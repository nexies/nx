//
// Created by nexie on 24.03.2026.
//

#include <nx/asio/context/reactor_handle.hpp>
#include "context_impl.hpp"

namespace nx::asio
{
    reactor_handle::reactor_handle(io_context & ctx)
        : ctx_ {ctx}
        , interest_ { io_interest::none }
        , handle_ { g_null_handle }
        , installed_ { false }
    {

    }

    reactor_handle::~reactor_handle()
    {
        uninstall();
    }

    void reactor_handle::install(native_handle_t handle, io_interest interest)
    {
        if (installed_)
            throw std::logic_error ("reactor_handle::install(): already installed");

        ctx_.impl_->register_reactor_handle(handle, this, interest);
        handle_ = handle;
        interest_ = interest;
        installed_ = true;
    }

    void reactor_handle::modify(native_handle_t handle, io_interest interest)
    {
        if (!installed_)
            throw std::logic_error ("reactor_handle::modify(): not installed");

        ctx_.impl_->modify_reactor_handle(handle, this, interest);
        handle_ = handle;
        interest_ = interest;
    }

    void reactor_handle::uninstall()
    {
        if (!installed_)
            return;
        ctx_.impl_->unregister_reactor_handle(handle_);
        handle_ = g_null_handle;
        interest_ = io_interest::none;
        installed_ = false;
    }

    io_interest reactor_handle::interest() const
    {
        return interest_;
    }

    native_handle_t reactor_handle::handle() const
    {
        return handle_;
    }

    bool reactor_handle::installed() const
    {
        return installed_;
    }

    io_context& reactor_handle::ctx() const
    {
        return ctx_;
    }
}
