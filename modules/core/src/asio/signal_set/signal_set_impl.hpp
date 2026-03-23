//
// Created by nexie on 23.03.2026.
//

#ifndef NX_CORE_ASIO_SIGNAL_SET_IMPL_HPP
#define NX_CORE_ASIO_SIGNAL_SET_IMPL_HPP

#include <nx/core/asio/signal_set.hpp>
#include <nx/core/asio/backend/reactor_handle.hpp>

namespace nx::asio
{

    class SignalSet::Impl : public ReactorHandler,
                            public std::enable_shared_from_this<Impl>
    {
    public:
        explicit
        Impl(Context & ctx);

        ~Impl ();

        void
        add(int signal);

        virtual void
        remove(int signal);

        void
        clear ();

        void
        asyncWait(HandlerType h);

        std::size_t
        cancel();

        void
        react(IOEvent event) override;

    private:
        void rebuild_fd();
        void close_fd();
        void handle_readable();

    private:
        Context & ctx_;
        NativeHandle fd_ { -1 };
        sigset_t mask_ { };
        bool waiting_ { false };
        HandlerType handler_ { nullptr };
    };
}

#endif //NX_CORE_ASIO_SIGNAL_SET_IMPL_HPP