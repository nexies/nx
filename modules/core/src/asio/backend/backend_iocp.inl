#if !defined(NX_CORE_ASIO_BACKEND_IOCP_INL) && defined(_WIN32)
#define NX_CORE_ASIO_BACKEND_IOCP_INL

#include <windows.h>
#include <iostream>
#include <stdexcept>
#include <memory>

namespace my_async {

using native_handle = HANDLE;

enum class io_interest : uint32_t {
    none  = 0,
    read  = 1u << 0,
    write = 1u << 1,
};

enum class io_event : uint32_t {
    none   = 0,
    read   = 1u << 0,
    write  = 1u << 1,
    error  = 1u << 2,
    hangup = 1u << 3,
    wakeup = 1u << 4,
    timer  = 1u << 5,
};

struct backend_event {
    void* token = nullptr;
    io_event events = io_event::none;
};

class backend {
public:
    virtual ~backend() = default;

    virtual void add(native_handle handle, void* token, io_interest interests) = 0;
    virtual void modify(native_handle handle, void* token, io_interest interests) = 0;
    virtual void remove(native_handle handle) = 0;

    virtual void wake() = 0;
    virtual std::size_t wait(backend_event* out, std::size_t capacity, std::optional<std::chrono::steady_clock::duration> timeout) = 0;
};

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

    void add(native_handle handle, void* token, io_interest interests) override {
        if (CreateIoCompletionPort(handle, iocp_handle_, reinterpret_cast<ULONG_PTR>(token), 0) == nullptr) {
            throw std::runtime_error("CreateIoCompletionPort failed for add");
        }
    }

    void modify(native_handle handle, void* token, io_interest interests) override {
        // На Windows не требуется модификация IOCP (в отличие от epoll)
    }

    void remove(native_handle handle) override {
        // Для IOCP нельзя "удалить" дескриптор, как с epoll, но можно закрыть handle
        CloseHandle(handle);
    }

    void wake() override {
        // В Windows IOCP не использует eventfd, но можно просто создать Wakeup дескриптор
        // Для демонстрации можно использовать сигнализацию через объект событий или манипулировать завершением работы.
    }

    std::size_t wait(backend_event* out, std::size_t capacity, std::optional<std::chrono::steady_clock::duration> timeout) override {
        DWORD bytesTransferred;
        ULONG_PTR completionKey;
        LPOVERLAPPED overlapped;

        int timeout_ms = timeout ? std::chrono::duration_cast<std::chrono::milliseconds>(*timeout).count() : INFINITE;
        BOOL result = GetQueuedCompletionStatus(iocp_handle_, &bytesTransferred, &completionKey, &overlapped, timeout_ms);

        if (!result) {
            return 0;
        }

        backend_event ev;
        ev.token = reinterpret_cast<void*>(completionKey);

        // Заполняем события в зависимости от типа операции
        // Предполагаем, что в completionKey храним данные о событии (например, чтение/запись)

        out[0] = ev;
        return 1;
    }

private:
    native_handle iocp_handle_;
};

} // namespace my_async

#endif