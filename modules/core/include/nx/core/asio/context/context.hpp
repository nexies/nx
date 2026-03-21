//
// Created by nexie on 21.03.2026.
//

#ifndef NX_ASIO_CONTEXT_HPP
#define NX_ASIO_CONTEXT_HPP

#include <functional>
#include <nx/core/asio/backend/backend_types.hpp>

namespace nx::asio {

    class Backend;

    class Context {
    public:
        using Task = std::function<void()>;

        Context ();
        ~Context ();

        void
        post (Task task);

        void
        dispatch (Task task);

        std::size_t
        run ();
        std::size_t
        runOnce ();
        std::size_t
        runFor (Duration duration);
        std::size_t
        runUntil (TimePoint time_point);

        std::size_t
        poll ();
        std::size_t
        pollOnce ();

        void
        stop ();
        void
        restart ();

        [[nodiscard]] bool
        stopped () const noexcept;

        [[nodiscard]] bool
        runningInThisThread () const noexcept;

    private:
        class Impl;
        std::unique_ptr<Impl> impl_;
    };
}

#endif //NX_ASIO_CONTEXT_HPP
