//
// Created by nexie on 23.03.2026.
//

#ifndef NX_HANDLE_NOTIFIER_IMPL_HPP
#define NX_HANDLE_NOTIFIER_IMPL_HPP

#include <nx/asio/context/io_context.hpp>
#include <nx/asio/handle_notifier.hpp>

#include "nx/asio/context/reactor_handle.hpp"

namespace nx::asio
{
    class handle_notifier::impl : public reactor_handle,
                                  public std::enable_shared_from_this<impl>
    {
        HandlerType handler_;
    public:

        explicit
        impl (io_context& ctx);

        void
        async_wait (HandlerType callback);

        void
        cancel ();

        void
        react(io_event event) override;

    private:
        void handle_event(io_event ev);
    };
}

#endif //NX_HANDLE_NOTIFIER_IMPL_HPP