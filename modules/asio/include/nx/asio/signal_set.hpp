//
// Created by nexie on 23.03.2026.
//

#ifndef NX_CORE_ASIO_SIGNAL_SET_HPP
#define NX_CORE_ASIO_SIGNAL_SET_HPP

#include <nx/common/types.hpp>

#include <functional>
#include <memory>
#include <csignal>

namespace nx::asio
{
    class io_context;

    class signal_set
    {
    public:
        using handler_type = std::function<void(nx::result<int>)>;

        explicit
        signal_set(io_context & ctx);

        ~signal_set();

        signal_set(const signal_set &) = delete;
        signal_set & operator=(const signal_set &) = delete;

        void add(int signal);
        void remove(int signal);
        void clear();

        template <typename Handler>
        void async_wait(Handler && handler)
        {
            _async_wait_impl(handler_type(std::forward<Handler>(handler)));
        }

        std::size_t cancel();

    private:
        void _async_wait_impl(handler_type handler);

        friend class signal_set_posix;
        friend class signal_set_kevent;
        class impl;
        std::shared_ptr<impl> impl_;
    };
}

#endif //NX_SIGNAL_SET_HPP