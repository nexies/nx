//
// Created by nexie on 21.03.2026.
//

#include <nx/core/asio/backend/backend.hpp>
#include "backend_epoll.inl"
#include "backend_kevent.inl"
#include "backend_iocp.inl"

namespace nx::asio {
    std::unique_ptr<Backend> Backend::CreateBackend() {
#if defined(NX_CORE_ASIO_BACKEND_EPOLL_INL)
        return std::make_unique<BackendEpoll>();
#elif defined (NX_CORE_ASIO_BACKEND_KEVENT_INL)
        // return std::make_unique<>()
#endif
        return nullptr;
    }
}
