#if !defined(NX_CORE_ASIO_BACKEND_KEVENT_INL) && defined(NX_OS_APPLE)
#define NX_CORE_ASIO_BACKEND_KEVENT_INL

#include <iostream>
#include <sys/types.h>
#include <sys/event.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdexcept>
#include <memory>
#include <chrono>
#include <optional>
#include <deque>

#include <nx/asio/backend/backend_types.hpp>

namespace nx::asio {

class kevent_backend : public backend {
public:
    kevent_backend() {
        kqueue_fd_ = kqueue();
        if (kqueue_fd_ == -1) {
            throw std::runtime_error("kqueue failed");
        }

        struct kevent wakeup;
        EV_SET(&wakeup, 1, EVFILT_USER, EV_ADD | EV_CLEAR, 0, 0, NULL);
        kevent(kqueue_fd_, &wakeup, 1, nullptr, 0, nullptr);
    }

    ~kevent_backend() override {
        if (kqueue_fd_ != -1)
            close(kqueue_fd_);
    }

    void add(native_handle_t handle, void* token, io_interest interests) override {
        kevent_update(handle, token, interests, EV_ADD | EV_ENABLE);
    }

    void modify(native_handle_t handle, void* token, io_interest interests) override {
        // kevent EV_ADD is idempotent — re-adding an existing filter updates it.
        // We add all filters in the new set; removing dropped filters is left to
        // remove() since we don't track the previous interests here.
        kevent_update(handle, token, interests, EV_ADD | EV_ENABLE);
    }

    void remove(native_handle_t handle, void * token, io_interest interest) override {
        kevent_update(handle, token, interest, EV_DELETE);
    }

    void wake() override {
        struct kevent ev;
        EV_SET(&ev, 1, EVFILT_USER, 0, NOTE_TRIGGER, 0, NULL);
        kevent(kqueue_fd_, &ev, 1, NULL, 0, NULL);
    }

    std::size_t wait(backend_event * out, std::size_t capacity, std::optional<std::chrono::steady_clock::duration> timeout) override {
        struct kevent events[capacity];
        struct timespec timeout_ts = { 0, 0 };

        if (timeout) {
            auto timeout_duration = std::chrono::duration_cast<std::chrono::milliseconds>(*timeout);
            timeout_ts.tv_sec = timeout_duration.count() / 1000;
            timeout_ts.tv_nsec = (timeout_duration.count() % 1000) * 1000000;
        }

        int event_count = kevent(kqueue_fd_, nullptr, 0, events, capacity, timeout ? &timeout_ts : nullptr);
        if (event_count == -1) {
            if (errno == EINTR) {
                // сигнал прервал ожидание — это ожидаемо
                // либо continue, либо graceful shutdown
            } else {
                perror("kevent");
            }

        }

        //TODO:
        for (int i = 0; i < event_count; ++i) {
            out[i].token = events[i].udata;
            if (events[i].filter == EVFILT_READ)
                out[i].events = io_event::read;
            else if (events[i].filter == EVFILT_WRITE)
                out[i].events = io_event::write;
            else if (events[i].filter == EVFILT_EXCEPT)
                out[i].events = io_event::error;
            else if (events[i].filter == EVFILT_USER)
                out[i].events = io_event::wakeup;
            else if (events[i].filter == EVFILT_SIGNAL) {
                out[i].events = io_event::read;
                out[i].data.signal.signum = events[i].ident;
                out[i].data.signal.reps = events[i].data;
            }
            else
                out[i].events = io_event::wakeup;
        }
        return event_count;
    }

private:
    void kevent_update(native_handle_t handle, void* token,
                       io_interest interests, int flags)
    {
        struct kevent ev;
        if ((interests & io_interest::read) != io_interest::none) {
            EV_SET(&ev, handle, EVFILT_READ, flags, 0, 0, token);
            if (kevent(kqueue_fd_, &ev, 1, nullptr, 0, nullptr) == -1)
                throw std::runtime_error("kevent failed (read filter)");
        }
        if ((interests & io_interest::write) != io_interest::none) {
            EV_SET(&ev, handle, EVFILT_WRITE, flags, 0, 0, token);
            if (kevent(kqueue_fd_, &ev, 1, nullptr, 0, nullptr) == -1)
                throw std::runtime_error("kevent failed (write filter)");
        }
        if ((interests & io_interest::signal) != io_interest::none) {
            EV_SET(&ev, handle, EVFILT_SIGNAL, flags, 0, 0, token);
            if (kevent(kqueue_fd_, &ev, 1, nullptr, 0, nullptr) == -1)
                throw std::runtime_error("kevent failed (signal filter)");
        }
    }

    native_handle_t kqueue_fd_;
};

} // namespace my_async


#endif