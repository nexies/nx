#if (!defined(NX_CORE_ASIO_BACKEND_EPOLL_INL)) && (defined(NX_OS_LINUX))
#define NX_CORE_ASIO_BACKEND_EPOLL_INL

#include <iostream>
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/eventfd.h>
#include <chrono>
#include <optional>
#include <thread>

#include <nx/asio/backend/backend.hpp>

// #include "nx/core/detail/units.hpp"

namespace nx::asio {
    class backend_epoll : public backend {
        native_handle_t epoll_fd_;
        native_handle_t event_fd_;
    public:
        backend_epoll () {
            epoll_fd_ = epoll_create1(0);
            if (epoll_fd_ == -1) {
                throw std::runtime_error("epoll_create1 failed");
            }

            event_fd_ = eventfd(0, /*EFD_NONBLOCK*/EFD_SEMAPHORE);
            if (event_fd_ == -1) {
                throw std::runtime_error("eventfd failed");
            }

            epoll_event ev { 0 };
            ev.events = EPOLLIN;
            ev.data.fd = event_fd_;

            if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, event_fd_, &ev) == -1) {
                throw std::runtime_error("epoll_ctl failed");
            }
        }

        ~backend_epoll() override {
            close(epoll_fd_);
            close(event_fd_);
        }

        void add(native_handle_t handle, void * token, io_interest interest) override {
            epoll_event ev {0};
            ev.events = 0;

            if ((interest & io_interest::read) != io_interest::none) {
                ev.events |= EPOLLIN;
            }
            if ((interest & io_interest::write) != io_interest::none) {
                ev.events |= EPOLLOUT;
            }

            ev.data.ptr = token;

            if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, handle, &ev) == -1) {
                throw std::runtime_error("add: epoll_ctl failed");
            }
        }

        void modify (native_handle_t handle, void * token, io_interest interest) override {
            epoll_event ev {0};
            ev.events = 0;

            if ((interest & io_interest::read) != io_interest::none)
                ev.events |= EPOLLIN;
            if ((interest & io_interest::write) != io_interest::none)
                ev.events |= EPOLLOUT;

            ev.data.ptr = token;

            if (epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, handle, &ev) == -1) {
                throw std::runtime_error("modify: epoll_ctl failed");
            }
        }

        void remove(native_handle_t handle) override {
            if (epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, handle, nullptr) == -1) {
                throw std::runtime_error("remove: epoll_ctl failed");
            }
        }

        void wake() override {
            std::uint64_t one = 1;
            if (write(event_fd_, &one, sizeof(one)) != sizeof(one)) {
                throw std::runtime_error("wake: write failed");
            }
        }

        std::size_t wait(backend_event *out, std::size_t capacity, std::optional<duration_type> timeout) override {
            int timeout_ms = timeout ? std::chrono::duration_cast<std::chrono::milliseconds>(timeout.value()).count() : -1;
            auto * events = static_cast<epoll_event *>(malloc(capacity * sizeof(epoll_event)));

            int n = epoll_wait(epoll_fd_, events, static_cast<int>(capacity), timeout_ms);

            if (n == -1) {
                throw std::runtime_error("wait: epoll_wait failed");
            }

            std::size_t event_count = 0;

            for (int i = 0; i < n; i++) {
                backend_event & ev = out[event_count++];
                ev.token = events[i].data.ptr;
                ev.identity = events[i].data.fd;
                ev.u32 = events[i].data.u32;
                ev.u64 = events[i].data.u64;

                if (events[i].events & EPOLLIN)
                    ev.events = io_event::Read;
                if (events[i].events & EPOLLOUT)
                    ev.events = io_event::Write;
                if (events[i].events & EPOLLERR)
                    ev.events = io_event::Error;
                if (events[i].events & EPOLLHUP)
                    ev.events = io_event::Hangup;
                if (events[i].events & EPOLLWAKEUP)
                    ev.events = io_event::Wakeup;
                if (events[i].data.fd == event_fd_)
                    ev.events = io_event::Wakeup;
            }

            free(events);
            return event_count;
        }
    };
}

#endif