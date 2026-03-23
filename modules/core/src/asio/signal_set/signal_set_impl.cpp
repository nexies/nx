//
// Created by nexie on 23.03.2026.
//

#include "signal_set_impl.hpp"

#include <sys/signalfd.h>
#include <signal.h>
#include <nx/core/detail/os_defs.hpp>

#include "../context/context_impl.hpp"
#include "nx/core/asio/context.hpp"


#if defined (NX_OS_LINUX)
namespace nx::asio
{
    SignalSet::Impl::Impl(Context& ctx)
        : ctx_{ctx}
    {
        sigemptyset(&mask_);
    }

    SignalSet::Impl::~Impl()
    {
        close_fd();
    }

    void SignalSet::Impl::add(int signal)
    {
        sigaddset(&mask_, signal);
        rebuild_fd();
    }

    void SignalSet::Impl::remove(int signal)
    {
        sigdelset(&mask_, signal);
        rebuild_fd();
    }

    void SignalSet::Impl::clear()
    {
        sigemptyset(&mask_);
        rebuild_fd();
    }

    void SignalSet::Impl::asyncWait(HandlerType h)
    {
        if (waiting_)
            throw std::logic_error ("signal_set already has pending async_wait");

        handler_ = h;
        waiting_ = true;
    }

    std::size_t SignalSet::Impl::cancel()
    {
        if (!waiting_)
            return 0;

        waiting_ = false;
        handler_ = nullptr;
        return 1;
    }

    void SignalSet::Impl::react(IOEvent event)
    {
        if ((event & IOEvent::Read) != IOEvent::None)
            handle_readable();
    }

    void SignalSet::Impl::rebuild_fd()
    {
        if (fd_ != -1)
        {
            ctx_.impl_->unregisterReactorHandle(fd_);
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

        ctx_.impl_->registerReactorHandle(fd_, this, IOInterest::Read);
    }

    void SignalSet::Impl::close_fd()
    {
        if (fd_ != -1)
        {
            ctx_.impl_->unregisterReactorHandle(fd_);
            ::close (fd_);
            fd_ = -1;
        }
    }

    void SignalSet::Impl::handle_readable()
    {
        signalfd_siginfo info {};
        auto n = ::read (fd_, &info, sizeof(info));
        if (n != sizeof(info))
            return;

        if (!waiting_) {
            return;
        }

        auto h = std::move(handler_);
        handler_ = nullptr;
        waiting_ = false;

        int signo = static_cast<int> (info.ssi_signo);
        ctx_.post([h = std::move(h), signo]() mutable
        {
            h(signo);
        });

    }
}

#else
# error "SignalSet::Impl is only defined on Linux"
#endif
