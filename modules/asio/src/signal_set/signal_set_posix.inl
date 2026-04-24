#if !defined(NX_ASIO_SIGNAL_SET_POSIX_IMPL) && defined(NX_OS_LINUX)
#define NX_ASIO_SIGNAL_SET_POSIX_IMPL

#if defined(NX_OS_APPLE)
#include "unistd.h"
#endif
#if defined(NX_OS_LINUX)
#include <sys/signalfd.h>
#endif


#include "signal.h"
#include "signal_set_impl.hpp"

#include <nx/asio/context/reactor_handle.hpp>

namespace nx::asio
{
    class signal_set_posix : public signal_set::impl,
                             public reactor_handle
    {
    public:
        using handler_type = signal_set::handler_type;

        explicit
        signal_set_posix(io_context &ctx) :
            reactor_handle(ctx)
        {
            sigemptyset(&mask_);
        }

        ~signal_set_posix () override {
            // close_fd();
            auto n = cancel();
        }

        void
        add(int signal) override {
            sigaddset(&mask_, signal);
            rebuild_fd();
        }

        void
        remove(int signal) override {
            sigdelset(&mask_, signal);
            rebuild_fd();
        }

        void
        clear () override {
            sigemptyset(&mask_);
            rebuild_fd();
        }

        void
        async_wait(handler_type h) override {
            if (handler_ != nullptr)
                throw std::runtime_error("signal_set already has pending await");

            handler_ = std::move(h);
        }

        std::size_t
        cancel() override {
            if (!installed())
                return 0;

            handler_ = nullptr;
            return 1;
        }

        void
        on_event(backend_event &event) override {
#if defined(NX_OS_APPLE)
            if (event.events == io_event::error) {
                // TODO: handle error
                return;
            }

            int signum = static_cast<int>(event.u32);
            if (handler_ == nullptr)
                return;

            ctx().post([signum = signum, handler = std::move(handler_)] {
               handler(signum);
            });
            handler_ = nullptr;
#elif defined(NX_OS_LINUX)
            handle_readable();
#endif
        }

    private:
        void rebuild_fd() {
            close_fd();

            sigset_t nulset;
            sigemptyset(&nulset);

            if (std::memcmp(&mask_, &nulset, sizeof(mask_)) == 0)
                return;

            // TODO: save old mask and restore it later
            if (::pthread_sigmask(SIG_SETMASK, &mask_, NULL) != 0) {
                throw std::runtime_error("pthread_sigmask failed");
            }

#if defined(NX_OS_LINUX)
            fd_ = ::signalfd(-1, &mask_, SFD_NONBLOCK | SFD_CLOEXEC);
            install(fd_, io_interest::read);
#elif defined(NX_OS_APPLE)
            fd_ = mask_;
            install(fd_, io_interest::signal);
#endif
        }

        void close_fd() {
            if (fd_ != g_null_handle) {
                uninstall();
#if defined(NX_OS_LINUX)
                ::close(fd_);
#endif
                fd_ = g_null_handle;
            }
        }

#if defined(NX_OS_LINUX)
        void handle_readable() {
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
#endif

#if defined(NX_OS_APPLE)
        void handle_not_readable()
        {

        }
#endif

    private:
        native_handle_t fd_ { g_null_handle };
        // bool waiting_ { false };
        sigset_t mask_ { };
        handler_type handler_ { nullptr };
    };
}



#endif