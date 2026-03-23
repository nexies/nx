#if(!defined(NX_CORE_ASIO_BACKEND_EPOLL_INL))&&(defined(__unix__))
#define NX_CORE_ASIO_BACKEND_EPOLL_INL

#include <iostream>
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/eventfd.h>
#include <chrono>
#include <optional>
#include <thread>

#include <nx/core/asio/backend/backend.hpp>

#include "nx/core/detail/units.hpp"

namespace nx::asio {
    class EpollBackend : public Backend {
        NativeHandle epoll_fd_;
        NativeHandle event_fd_;
    public:
        EpollBackend () {
            epoll_fd_ = epoll_create1(0);
            if (epoll_fd_ == -1) {
                throw std::runtime_error("epoll_create1 failed");
            }

            event_fd_ = eventfd(0, EFD_NONBLOCK);
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

        ~EpollBackend() override {
            close(epoll_fd_);
            close(event_fd_);
        }

        void add(NativeHandle handle, void * token, IOInterest interest) override {
            epoll_event ev {0};
            ev.events = 0;

            if ((interest & IOInterest::Read) != IOInterest::None) {
                ev.events |= EPOLLIN;
            }
            if ((interest & IOInterest::Write) != IOInterest::None) {
                ev.events |= EPOLLOUT;
            }

            ev.data.ptr = token;

            if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, handle, &ev) == -1) {
                throw std::runtime_error("add: epoll_ctl failed");
            }
        }

        void modify (NativeHandle handle, void * token, IOInterest interest) override {
            epoll_event ev {0};
            ev.events = 0;

            if ((interest & IOInterest::Read) != IOInterest::None)
                ev.events |= EPOLLIN;
            if ((interest & IOInterest::Write) != IOInterest::None)
                ev.events |= EPOLLOUT;

            ev.data.ptr = token;

            if (epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, handle, &ev) == -1) {
                throw std::runtime_error("modify: epoll_ctl failed");
            }
        }

        void remove(NativeHandle handle) override {
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

        std::size_t wait(BackendEvent *out, std::size_t capacity, std::optional<duration_type> timeout) override {
            int timeout_ms = timeout ? std::chrono::duration_cast<Milliseconds>(timeout.value()).count() : -1;
            auto * events = static_cast<epoll_event *>(malloc(capacity * sizeof(epoll_event)));

            int n = epoll_wait(epoll_fd_, events, static_cast<int>(capacity), timeout_ms);

            if (n == -1) {
                throw std::runtime_error("wait: epoll_wait failed");
            }

            std::size_t event_count = 0;

            for (int i = 0; i < n; i++) {
                BackendEvent & ev = out[event_count++];
                ev.token = events[i].data.ptr;
                ev.identity = events[i].data.fd;
                ev.u32 = events[i].data.u32;
                ev.u64 = events[i].data.u64;

                if (events[i].events & EPOLLIN)
                    ev.events = IOEvent::Read;
                if (events[i].events & EPOLLOUT)
                    ev.events = IOEvent::Write;
                if (events[i].events & EPOLLERR)
                    ev.events = IOEvent::Error;
                if (events[i].events & EPOLLHUP)
                    ev.events = IOEvent::Hangup;
                if (events[i].events & EPOLLWAKEUP)
                    ev.events = IOEvent::Wakeup;
                if (events[i].data.fd == event_fd_)
                    ev.events = IOEvent::Wakeup;
            }

            free(events);
            return event_count;
        }
    };
}

#endif