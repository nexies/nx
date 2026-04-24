//
// Created by nexie on 23.03.2026.
//

#ifndef NX_CORE_ASIO_SIGNAL_SET_IMPL_HPP
#define NX_CORE_ASIO_SIGNAL_SET_IMPL_HPP

#include <nx/asio/signal_set.hpp>
#include <nx/asio/context/reactor_handle.hpp>

namespace nx::asio
{
    class signal_set::impl : public std::enable_shared_from_this<impl>
    {
    public:
        virtual
        ~impl () = default;

        virtual void
        add(int signal) = 0;

        virtual void
        remove(int signal) = 0;

        virtual void
        clear () = 0;

        virtual void
        async_wait(handler_type h) = 0;

        virtual std::size_t
        cancel() = 0;

        static std::shared_ptr<impl>
        create_impl(io_context & ctx);
    };
}

#endif //NX_CORE_ASIO_SIGNAL_SET_IMPL_HPP