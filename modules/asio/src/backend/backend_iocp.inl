#if !defined(NX_CORE_ASIO_BACKEND_IOCP_INL) && defined(NX_OS_WINDOWS)
#define NX_CORE_ASIO_BACKEND_IOCP_INL

#include <nx/asio/backend/backend.hpp>

#include <windows.h>
#include <iostream>
#include <stdexcept>
#include <memory>

static constexpr ULONG_PTR kWakeKey = 1;
static constexpr ULONG_PTR kStopKey = 2;

namespace nx::asio {

class iocp_backend : public backend {
public:
    iocp_backend() {
        // Создаем IOCP
        iocp_handle_ = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);
        if (iocp_handle_ == nullptr) {
            throw std::runtime_error("CreateIoCompletionPort failed");
        }
    }

    ~iocp_backend() {
        CloseHandle(iocp_handle_);
    }

    void add(native_handle_t handle, void* token, io_interest interests) override {
        if (CreateIoCompletionPort(handle, iocp_handle_, reinterpret_cast<ULONG_PTR>(token), 0) == nullptr) {
            throw std::runtime_error("CreateIoCompletionPort failed for add");
        }
    }

    void modify(native_handle_t handle, void* token, io_interest interests) override {
        // На Windows не требуется модификация IOCP (в отличие от epoll)
    }

    void remove(native_handle_t handle, void* token, io_interest interests) override {
        // Для IOCP нельзя "удалить" дескриптор, как с epoll, но можно закрыть handle
        CloseHandle(handle);
    }

    void wake() override {
        ::PostQueuedCompletionStatus(iocp_handle_, 0, kWakeKey, nullptr);
    }

    std::size_t wait(backend_event* out, std::size_t capacity,
                     std::optional<std::chrono::steady_clock::duration> timeout) override
    {
        DWORD       bytesTransferred {};
        ULONG_PTR   completionKey    {};
        LPOVERLAPPED overlapped      {};

        int timeout_ms = timeout
            ? static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(*timeout).count())
            : INFINITE;

        BOOL ok = GetQueuedCompletionStatus(
            iocp_handle_, &bytesTransferred, &completionKey, &overlapped, timeout_ms);

        if (!ok)
            return 0; // timeout or error

        backend_event ev {};
        if (completionKey == kWakeKey && overlapped == nullptr) {
            ev.events = io_event::wakeup;
        }
        // completionKey == token pointer for reactor handles (future use)
        ev.token  = reinterpret_cast<void*>(completionKey);
        out[0]    = ev;
        return 1;
    }

private:
    native_handle_t iocp_handle_;
};

} // namespace my_async

#endif