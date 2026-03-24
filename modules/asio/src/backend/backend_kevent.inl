#include <nx/common/platform.hpp>

#if !defined(NX_CORE_ASIO_BACKEND_KEVENT_INL) && defined(NX_OS_MACOS)
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

namespace nx::asio {

class KEventBackend : public Backend {
public:
    KEventBackend() {
        // Создаём kqueue
        kqueue_fd_ = kqueue();
        if (kqueue_fd_ == -1) {
            throw std::runtime_error("kqueue failed");
        }
    }

    ~KEventBackend() override {
        if (kqueue_fd_ != -1)
            close(kqueue_fd_);
    }

    void add(NativeHandle handle, void* token, IOInterest interests) override {
        struct kevent ev;
        int filter = 0;
        if ((interests & IOInterest::Read) != IOInterest::None) {
            filter |= EVFILT_READ;
        }
        if ((interests & IOInterest::Write) != IOInterest::None) {
            filter |= EVFILT_WRITE;
        }

        EV_SET(&ev, handle, filter, EV_ADD | EV_ENABLE, 0, 0, token);
        if (kevent(kqueue_fd_, &ev, 1, nullptr, 0, nullptr) == -1) {
            throw std::runtime_error("kevent failed for add");
        }
    }

    void modify(NativeHandle handle, void* token, IOInterest interests) override {
        struct kevent ev;
        int filter = 0;
        if ((interests & IOInterest::Read) != IOInterest::None) {
            filter |= EVFILT_READ;
        }
        if ((interests & IOInterest::Write) != IOInterest::None) {
            filter |= EVFILT_WRITE;
        }

        EV_SET(&ev, handle, filter, EV_ADD | EV_ENABLE, 0, 0, token);
        if (kevent(kqueue_fd_, &ev, 1, nullptr, 0, nullptr) == -1) {
            throw std::runtime_error("kevent failed for modify");
        }
    }

    void remove(NativeHandle handle) override {
        struct kevent ev;
        EV_SET(&ev, handle, EVFILT_READ, EV_DELETE, 0, 0, nullptr);
        if (kevent(kqueue_fd_, &ev, 1, nullptr, 0, nullptr) == -1) {
            throw std::runtime_error("kevent failed for remove");
        }
    }

    void wake() override {
        // uint64_t one = 1;
        // if (write(event_fd_, &one, sizeof(one)) != sizeof(one)) {
            // throw std::runtime_error("Failed to wakeup eventfd");
        // }
    }

    std::size_t wait(BackendEvent* out, std::size_t capacity, std::optional<std::chrono::steady_clock::duration> timeout) override {
        struct kevent events[capacity];
        struct timespec timeout_ts = { 0, 0 };

        if (timeout) {
            auto timeout_duration = std::chrono::duration_cast<std::chrono::milliseconds>(*timeout);
            timeout_ts.tv_sec = timeout_duration.count() / 1000;
            timeout_ts.tv_nsec = (timeout_duration.count() % 1000) * 1000000;
        }

        int event_count = kevent(kqueue_fd_, nullptr, 0, events, capacity, timeout ? &timeout_ts : nullptr);
        if (event_count == -1) {
            throw std::runtime_error("Failed to wait for events");
        }

        //TODO:
        for (int i = 0; i < event_count; ++i) {
            // out[i].events. = events[i].ident;
            out[i].token = events[i].udata;
            // out[i].events = (events[i].filter == EVFILT_READ) ? IOEvent::Read : IOInterest::Write;
            if (events[i].filter == EVFILT_READ)
                out[i].events = IOEvent::Read;
            else if (events[i].filter == EVFILT_WRITE)
                out[i].events = IOEvent::Write;
            else if (events[i].filter == EVFILT_EXCEPT)
                out[i].events = IOEvent::Error;
            // if (events[i].filter == EVFILT_)
            // if (events[i].)
            else
                out[i].events = IOEvent::Wakeup;
        }
        return event_count;
    }

private:
    NativeHandle kqueue_fd_;
};

} // namespace my_async


#endif