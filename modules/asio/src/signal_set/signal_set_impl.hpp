//
// Created by nexie on 23.03.2026.
//

#ifndef NX_CORE_ASIO_SIGNAL_SET_IMPL_HPP
#define NX_CORE_ASIO_SIGNAL_SET_IMPL_HPP

#include <nx/asio/signal_set.hpp>
#include <nx/asio/context/reactor_handle.hpp>

namespace nx::asio
{

    class signal_set::impl : public reactor_handle,
                             public std::enable_shared_from_this<impl>
    {
    public:
        explicit
        impl(io_context & ctx);

        ~impl ();

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
        react(io_event event) override;

    private:
        void rebuild_fd();
        void close_fd();
        void handle_readable();

    private:
        // io_context & ctx_;
        native_handle_t fd_ { g_null_handle };
        sigset_t mask_ { };
        // bool waiting_ { false };
        HandlerType handler_ { nullptr };
    };
}

#endif //NX_CORE_ASIO_SIGNAL_SET_IMPL_HPP