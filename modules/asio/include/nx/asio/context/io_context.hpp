//
// Created by nexie on 21.03.2026.
//

#ifndef NX_ASIO_CONTEXT_HPP
#define NX_ASIO_CONTEXT_HPP

#include <functional>
#include <memory>
#include <nx/asio/backend/backend_types.hpp>

namespace nx::asio {

    class backend;
    class steady_timer;

    class io_context {
    public:
        friend class reactor_handle;
        friend class steady_timer;
        friend class signal_set;
        friend class handle_notifier;

        using task_t = std::function<void()>;

        io_context ();
        ~io_context ();

        void
        post (task_t task);

        void
        dispatch (task_t task);

        std::size_t
        run ();

        std::size_t
        run_once ();

        std::size_t
        run_for (duration duration);

        std::size_t
        run_until (time_point time_point);

        std::size_t
        poll ();

        std::size_t
        poll_once ();

        void
        stop ();

        void
        restart ();

        [[nodiscard]] bool
        stopped () const noexcept;

        [[nodiscard]] bool
        is_running_in_this_thread () const noexcept;

    private:
        class impl;
        std::unique_ptr<impl> impl_;
    };
}

#endif //NX_ASIO_CONTEXT_HPP
