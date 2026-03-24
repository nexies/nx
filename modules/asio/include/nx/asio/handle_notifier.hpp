//
// Created by nexie on 23.03.2026.
//

#ifndef NX_HANDLE_NOTIFIER_HPP
#define NX_HANDLE_NOTIFIER_HPP

#include <functional>
#include <nx/asio/context/io_context.hpp>

namespace nx::asio
{
    class handle_notifier
    {
    public:
        using HandlerType = std::function<void (native_handle_t, io_event)>;

        explicit
        handle_notifier(io_context & context);
        handle_notifier(io_context & context, native_handle_t handle, io_interest interest);
        ~handle_notifier() = default;

        handle_notifier(const handle_notifier &) = delete;
        handle_notifier & operator=(const handle_notifier &) = delete;

        void
        setHandle (native_handle_t handle);

        void
        setInterest (io_interest interest);

        [[nodiscard]] native_handle_t
        handle () const;

        [[nodiscard]] io_interest
        interest () const;

        template <typename Handler>
        void
        async_wait(Handler && handler)
        {
            _async_wait_impl(HandlerType(std::forward<Handler>(handler)));
        }

        void
        cancel ();

    private:
        void _async_wait_impl(HandlerType handler);

        class impl;
        std::shared_ptr<impl> impl_;
    };
}

#endif //NX_HANDLE_NOTIFIER_HPP