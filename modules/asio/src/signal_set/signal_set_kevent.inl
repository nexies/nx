#if !defined(NX_ASIO_SIGNAL_SET_KEVENT_IMPL) && defined(NX_OS_APPLE)
#define NX_ASIO_SIGNAL_SET_KEVENT_IMPL

#include "unistd.h"

#include "signal.h"
#include "signal_set_impl.hpp"

#include <nx/asio/context/reactor_handle.hpp>

namespace nx::asio
{
    class signal_set_kevent : public signal_set::impl
    {
        class reactor : public reactor_handle {

            signal_set_kevent * obj_;
            int signal;
        public:
            reactor(io_context & ctx, int signal, signal_set_kevent * p)
                : reactor_handle(ctx)
                , obj_ { p }
                , signal { signal }
            {
                sigset_t set;
                sigaddset(&set, signal);
                ::pthread_sigmask(SIG_BLOCK, &set, NULL);
                install(signal, io_interest::signal);
            }

            ~reactor() override {
                // obj_->remove_from_pthread(signal);
                sigset_t set;
                sigaddset(&set, signal);
                ::pthread_sigmask(SIG_UNBLOCK, &set, NULL);
                // uninstall();
            }

            void
            on_event(backend_event &event) override {
                obj_->on_signal(event.data.signal.signum, event.data.signal.reps);
            }

        };

    public:
        using handler_type = signal_set::handler_type;

        explicit
        signal_set_kevent(io_context &ctx)
            : ctx_ { ctx }
        {
        }

        ~signal_set_kevent () override {
            auto n = cancel();
        }

        void
        add(int signal) override {
            if (reactors_.find(signal) == reactors_.end()) {
                reactors_.try_emplace(signal, ctx_, signal, this);
            }
        }

        void
        remove(int signal) override {
            if (reactors_.find(signal) != reactors_.end())
                reactors_.erase(signal);
        }

        void
        clear () override {
            reactors_.clear();
        }

        void
        async_wait(handler_type h) override {
            if (handler_ != nullptr)
                throw std::runtime_error("signal_set already has pending await");

            handler_ = std::move(h);
        }

        std::size_t
        cancel() override {
            handler_ = nullptr;
            return 1;
        }

        void
        on_signal(int signal, int reps) {
            if (handler_) {
                ctx_.post([handler = std::move(handler_), signum = signal] {
                   handler(signum);
                });
                handler_ = nullptr;
            }
        }

        void add_to_pthread(int signum) {
            sigaddset(&mask_, signum);
            sigset_t nullset;
            sigemptyset(&nullset);
            int res = 0;

            if (original_mask == nullset) {
                res = ::pthread_sigmask(SIG_SETMASK, &mask_, &original_mask);
            }
            else {
                res = ::pthread_sigmask(SIG_SETMASK, &mask_, NULL);
            }

            if (res != 0)
                throw std::runtime_error("pthread_sigmask failed");
        }

        void remove_from_pthread(int signum) {
            sigdelset(&mask_, signum);
            sigset_t nullset;
            sigemptyset(&nullset);
            int res = 0;

            if (mask_ == nullset) {
                res = ::pthread_sigmask(SIG_SETMASK, &original_mask, NULL);
            }
            else {
                res = ::pthread_sigmask(SIG_SETMASK, &mask_, NULL);
            }

            if (res != 0)
                throw std::runtime_error("pthread_sigmask failed");
        }
    private:
        sigset_t original_mask;
        sigset_t mask_;

        io_context & ctx_;
        handler_type handler_;
        std::unordered_map<int, reactor> reactors_;
    };
}



#endif