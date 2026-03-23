//
// Created by nexie on 21.03.2026.
//

#ifndef NX_ASIO_CONTEXT_IMPL_HPP
#define NX_ASIO_CONTEXT_IMPL_HPP

#include <nx/core/asio/context.hpp>
#include <nx/core.hpp>

#include "nx/core/asio/backend/backend_types.hpp"

namespace nx::asio {


    class Context::Impl {
    public:
        explicit Impl(std::unique_ptr<Backend> backend);
        ~Impl();

        void
        post (Task task);
        void
        dispatch (Task task);

        std::size_t
        run (std::optional<Duration> max_duration);

        std::size_t
        runOnce ();

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

        [[nodiscard]] nx::TimerId
        createTimer(TimePoint expiry, Task task);
        void
        cancelTimer(TimerId id);

        void
        registerReactorHandle(NativeHandle handle, void * token, IOInterest interest);
        void
        modifyReactorHandle(NativeHandle handle, void * token, IOInterest interest);
        void
        unregisterReactorHandle(NativeHandle handle);

    private:
        void
        drainIncoming ();

        void
        drainExpiredTimers ();

        [[nodiscard]] std::optional<Duration>
        computeWaitTimeout (std::optional<Duration> max_duration) const;

        std::size_t
        executeReady ();

        void
        processBackendEvents (BackendEvent * events, std::size_t count);

    private:
        std::unique_ptr<Backend> backend_;
        mutable std::mutex incoming_mutex_;
        std::deque<Task> incoming_;
        std::deque<Task> ready_;


        mutable std::mutex state_mutex_;
        std::atomic_bool stopped_ { false };
        std::atomic_bool running_ { false };
        std::thread::id owner_thread_id {};

        struct TimerOp {
            TimePoint expiry;
            Task task;
            bool canceled;
            TimerId id;
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
        TimerId next_timer_id_ { 0 };

    };
}



#endif //NX_ASIO_CONTEXT_IMPL_HPP
