//
// Created by nexie on 21.03.2026.
//

#ifndef NX_ASIO_CONTEXT_IMPL_HPP
#define NX_ASIO_CONTEXT_IMPL_HPP

#include <nx/asio/context/io_context.hpp>

#include <mutex>
#include <deque>
#include <atomic>
#include <functional>
#include <thread>
#include <queue>
#include <memory>

#include "nx/common/helpers.hpp"

namespace nx::asio {


    class io_context::impl {
    public:
        explicit impl(std::unique_ptr<backend> backend);
        ~impl();

        void
        post (task_t task);
        void
        dispatch (task_t task);

        std::size_t
        run (std::optional<duration> max_duration);

        std::size_t
        run_once ();

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

        [[nodiscard]] timer_id
        create_timer(time_point expiry, task_t && task);
        void
        cancel_timer(timer_id id);

        void
        register_reactor_handle(native_handle_t handle, void * token, io_interest interest);
        void
        modify_reactor_handle(native_handle_t handle, void * token, io_interest interest);
        void
        unregister_reactor_handle(native_handle_t handle, void * token, io_interest interest);

    private:
        void
        drainIncoming ();

        void
        drainExpiredTimers ();

        [[nodiscard]] std::optional<duration>
        computeWaitTimeout (std::optional<duration> max_duration) const;

        std::size_t
        executeReady ();

        std::size_t
        execute_one_ready ();

        void
        processBackendEvents (backend_event * events, std::size_t count);


        NX_NODISCARD std::size_t
        _consume_wakeup_event (backend_event * event);

    private:
        std::unique_ptr<backend> backend_;
        mutable std::mutex incoming_mutex_;
        std::deque<task_t> incoming_;
        std::deque<task_t> ready_;


        mutable std::mutex state_mutex_;
        std::atomic_bool stopped_ { false };
        std::atomic_bool running_ { false };
        std::thread::id owner_thread_id {};

        struct TimerOp {
            time_point expiry;
            task_t task;
            bool canceled;
            timer_id id;

            TimerOp(time_point expiry, task_t && task, bool canceled, timer_id id)
                : expiry(expiry)
                , task(std::move(task))
                , canceled(canceled)
                , id(id)
            { }
        };

        // struct TimerOpCmp {
        //     bool operator < (const TimerOp & left, const TimerOp & right) {
        //         return left.expiry < right.expiry;
        //     }
        // };

        std::priority_queue<std::shared_ptr<TimerOp>,
                            std::vector<std::shared_ptr<TimerOp>>,
                            std::greater<>> timers_;

        std::unordered_map<std::uint64_t, std::shared_ptr<TimerOp>> timer_storage_;
        timer_id next_timer_id_ { 0 };

    };
}



#endif //NX_ASIO_CONTEXT_IMPL_HPP
