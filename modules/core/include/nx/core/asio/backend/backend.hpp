//
// Created by green on 3/21/2026.
//

#ifndef NX_ASIO_BACKEND_HPP
#define NX_ASIO_BACKEND_HPP

#include "backend_types.hpp"

namespace nx::asio {

    class Backend {
    public:
        using duration_type = std::chrono::steady_clock::duration;

        virtual ~Backend() = default;

        virtual void
        add (NativeHandle handle, void * token, IOInterest interest) = 0;

        virtual void
        modify (NativeHandle handle, void * token, IOInterest interest) = 0;

        virtual void
        remove (NativeHandle handle) = 0;

        virtual void
        wake () = 0;

        virtual std::size_t
        wait (BackendEvent * out, std::size_t capacity, std::optional<duration_type> timeout) = 0;

        virtual std::size_t
        poll (BackendEvent * out, std::size_t capacity);

        static std::unique_ptr<Backend> CreateBackend();
    };
}

#endif //NX_ASIO_BACKEND_HPP