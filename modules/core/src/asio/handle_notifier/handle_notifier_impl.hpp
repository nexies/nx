//
// Created by nexie on 23.03.2026.
//

#ifndef NX_HANDLE_NOTIFIER_IMPL_HPP
#define NX_HANDLE_NOTIFIER_IMPL_HPP

#include <nx/core/asio/context.hpp>
#include <nx/core/asio/handle_notifier.hpp>

#include "nx/core/asio/backend/reactor_handle.hpp"

namespace nx::asio
{
    class HandleNotifier::Impl : public ReactorHandler,
                                 public std::enable_shared_from_this<Impl>
    {
        Context & ctx_;
        NativeHandle fd_;
        IOInterest interest_;
        HandlerType handler_;
        bool is_waiting_;
        bool registered_;


    public:

        explicit
        Impl (Context& ctx);

        ~Impl();

        void
        setHandle (NativeHandle handle);

        void
        setInterest (IOInterest interest);

        [[nodiscard]] NativeHandle
        handle () const;

        [[nodiscard]] IOInterest
        interest () const;

        void
        cancel ();

        void
        asyncWait (HandlerType callback);

        void
        react(IOEvent event) override;

    private:
        void reg ();
        void unreg ();
        void handle_event(IOEvent ev);
    };
}

#endif //NX_HANDLE_NOTIFIER_IMPL_HPP