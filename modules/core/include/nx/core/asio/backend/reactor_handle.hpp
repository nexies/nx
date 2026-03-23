//
// Created by nexie on 23.03.2026.
//

#ifndef NX_REACTOR_HANDLE_HPP
#define NX_REACTOR_HANDLE_HPP

#include <nx/core/asio/backend/backend_types.hpp>

namespace nx::asio
{
    class ReactorHandler
    {
    public:

        virtual
        ~ReactorHandler() = default;

        virtual void
        react(IOEvent event) = 0;
    };
}

#endif //NX_REACTOR_HANDLE_HPP