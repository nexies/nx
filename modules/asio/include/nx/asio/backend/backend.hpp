//
// Created by green on 3/21/2026.
//

#ifndef NX_ASIO_BACKEND_HPP
#define NX_ASIO_BACKEND_HPP

#include "backend_types.hpp"

#include <memory>

#include "nx/optional.hpp"

NX_ASIO_NAMESPACE_BEGIN

    class backend {
    public:
        using duration_type = std::chrono::steady_clock::duration;

        virtual ~backend() = default;

        virtual void
        add (native_handle_t handle, void * token, io_interest interest) = 0;

        virtual void
        modify (native_handle_t handle, void * token, io_interest interest) = 0;

        virtual void
        remove (native_handle_t handle, void * token, io_interest interest) = 0;

        virtual void
        wake () = 0;

        virtual std::size_t
        wait (backend_event * out, std::size_t capacity, optional<duration_type> timeout) = 0;

        virtual std::size_t
        poll (backend_event * out, std::size_t capacity);
    };

    std::unique_ptr<backend>
    create_backend();

NX_ASIO_NAMESPACE_END

#endif //NX_ASIO_BACKEND_HPP