//
// Created by nexie on 23.03.2026.
//

#ifndef NX_HANDLE_NOTIFIER_HPP
#define NX_HANDLE_NOTIFIER_HPP

#include <functional>
#include <nx/core/asio/context.hpp>

namespace nx::asio
{
    class HandleNotifier
    {
    public:
        using HandlerType = std::function<void (NativeHandle, IOEvent)>;

        explicit
        HandleNotifier(Context & context);
        HandleNotifier(Context & context, NativeHandle handle, IOInterest interest);
        ~HandleNotifier() = default;

        HandleNotifier(const HandleNotifier &) = delete;
        HandleNotifier & operator=(const HandleNotifier &) = delete;

        void
        setHandle (NativeHandle handle);

        void
        setInterest (IOInterest interest);

        [[nodiscard]] NativeHandle
        handle () const;

        [[nodiscard]] IOInterest
        interest () const;

        template <typename Handler>
        void
        asyncWait(Handler && handler)
        {
            _asyncWaitImpl(HandlerType(std::forward<Handler>(handler)));
        }

        void
        cancel ();

    private:
        void _asyncWaitImpl(HandlerType handler);

        class Impl;
        std::shared_ptr<Impl> impl_;
    };
}

#endif //NX_HANDLE_NOTIFIER_HPP