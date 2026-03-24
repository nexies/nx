//
// Created by nexie on 23.03.2026.
//

#ifndef NX_REACTOR_HANDLE_HPP
#define NX_REACTOR_HANDLE_HPP

#include <nx/asio/backend/backend_types.hpp>
#include <nx/asio/context/io_context.hpp>

namespace nx::asio
{
    class reactor_handle
    {
    public:
        explicit
        reactor_handle(io_context & ctx);

        virtual
        ~reactor_handle();

        virtual void
        react(io_event event) = 0;

        void
        install (native_handle_t handle, io_interest interest);

        void
        modify (native_handle_t handle, io_interest interest);

        void
        uninstall();

        [[nodiscard]] io_interest
        interest () const;

        [[nodiscard]] native_handle_t
        handle () const;

        [[nodiscard]] bool
        installed () const;

        [[nodiscard]] io_context &
        ctx() const;

    private:
        io_context & ctx_;
        io_interest interest_;
        native_handle_t handle_;
        bool installed_;
    };

}

#endif //NX_REACTOR_HANDLE_HPP