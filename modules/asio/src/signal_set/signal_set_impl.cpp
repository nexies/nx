//
// Created by nexie on 23.03.2026.
//

#include <nx/common/platform.hpp>

#include "signal_set_impl.hpp"
#include <signal.h>

#include "../context/context_impl.hpp"

#if defined (NX_OS_LINUX)
#include <sys/signalfd.h>
namespace nx::asio
{
    signal_set::impl::impl(io_context& ctx)
        : reactor_handle(ctx)
    {
        sigemptyset(&mask_);
    }

    signal_set::impl::~impl()
    {
        close_fd();
    }

    void signal_set::impl::add(int signal)
    {
        sigaddset(&mask_, signal);
        rebuild_fd();
    }

    void signal_set::impl::remove(int signal)
    {
        sigdelset(&mask_, signal);
        rebuild_fd();
    }

    void signal_set::impl::clear()
    {
        sigemptyset(&mask_);
        rebuild_fd();
    }

    void signal_set::impl::asyncWait(HandlerType h)
    {
        if (handler_ != nullptr)
            throw std::logic_error ("signal_set already has pending async_wait");

        handler_ = std::move(h);
    }

    std::size_t signal_set::impl::cancel()
    {
        if (!installed())
            return 0;

        handler_ = nullptr;
        return 1;
    }

    void signal_set::impl::react(io_event event)
    {
        if ((event & io_event::Read) != io_event::None)
            handle_readable();
    }

    void signal_set::impl::rebuild_fd()
    {
        if (fd_ != g_null_handle)
        {
            // ctx().impl_->unregister_reactor_handle(handle());
            uninstall();
            ::close (fd_);
            fd_ = -1;
        }

        if (sigisemptyset(&mask_) != 0)
            return;

        if (::pthread_sigmask(SIG_SETMASK, &mask_, NULL) != 0)
        {
            throw std::runtime_error ("sigisemptyset failed");
        }

        fd_ = ::signalfd(-1, &mask_, SFD_NONBLOCK | SFD_CLOEXEC);
        if (fd_ == -1)
            throw std::runtime_error ("signalfd failed");

        // ctx_.impl_->register_reactor_handle(fd_, this, io_interest::read);
        install(fd_, io_interest::read);
    }

    void signal_set::impl::close_fd()
    {
        if (fd_ != -1)
        {
            // ctx_.impl_->unregister_reactor_handle(fd_);
            uninstall();
            ::close (fd_);
            fd_ = -1;
        }
    }

    void signal_set::impl::handle_readable()
    {
        signalfd_siginfo info {};
        auto n = ::read (fd_, &info, sizeof(info));
        if (n != sizeof(info))
            return;

        if (handler_ == nullptr) {
            return;
        }

        auto h = std::move(handler_);
        handler_ = nullptr;

        int signo = static_cast<int> (info.ssi_signo);
        ctx().post([h = std::move(h), signo]() mutable
        {
            h(signo);
        });

    }
}

#elif defined(NX_OS_APPLE)

#else
# error "SignalSet::Impl is only defined on Linux"
#endif
