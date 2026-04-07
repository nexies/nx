//
// Created by nexie on 21.03.2026.
//

#include <nx/asio/backend/backend.hpp>
#include "backend_epoll.inl"
#include "backend_kevent.inl"
#include "backend_iocp.inl"

namespace nx::asio {
    std::size_t backend::poll(backend_event *out, std::size_t capacity) {
        return wait(out, capacity, duration(0));
    }

std::unique_ptr<backend> create_backend() {
#if defined(NX_CORE_ASIO_BACKEND_EPOLL_INL)
        return std::make_unique<backend_epoll>();
#elif defined (NX_CORE_ASIO_BACKEND_KEVENT_INL)
        return std::make_unique<kevent_backend>();
#elif defined (NX_CORE_ASIO_BACKEND_IOCP_INL)
        return std::make_unique<iocp_backend>();
#endif
        return nullptr;
    }
}
