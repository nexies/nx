#if !defined(NX_CORE_ASIO_BACKEND_IOCP_INL) && defined(NX_OS_WINDOWS)
#define NX_CORE_ASIO_BACKEND_IOCP_INL

//
// backend_iocp — Windows IOCP backend for nx::asio.
//
// Two registration paths, both entered through the same add() call:
//
//   IOCP path  (sockets, files opened with FILE_FLAG_OVERLAPPED):
//     CreateIoCompletionPort associates the handle with our IOCP.
//     Completions arrive in wait() when the caller's overlapped operations
//     (WSARecv, WSASend, AcceptEx, …) finish.
//
//   Waitable path  (console, named pipes, Win32 events, sync objects):
//     RegisterWaitForSingleObject registers a one-shot NT thread-pool wait.
//     When the handle is signalled the pool callback posts a completion to our
//     IOCP, and wait() delivers it just like any other event.
//     Call modify() to re-arm the one-shot wait after each event.
//
// The caller (reactor_handle / handle_notifier) does not need to know which
// path was taken — the choice is made once in add() and is transparent.
//
// wait() uses GetQueuedCompletionStatusEx for batch processing, matching the
// epoll_wait / kevent semantics of the other backends.
//

#include <nx/asio/backend/backend.hpp>

#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#  define NOMINMAX
#endif
#include <windows.h>

#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace nx::asio {

// ── watch_entry ───────────────────────────────────────────────────────────────
//
// Bookkeeping for a handle on the waitable path.
// Owned by iocp_backend::watches_, keyed by native handle value.

struct watch_entry {
    HANDLE  iocp;      // back-reference to the IOCP (not owned)
    void  * token;     // reactor_handle* — used as IOCP completion key
    HANDLE  wait_reg;  // returned by RegisterWaitForSingleObject; null if not armed

    // NT thread-pool callback.  Executes (once) on a pool thread when the
    // watched handle is signalled.  Only PostQueuedCompletionStatus is called —
    // cheap, thread-safe, and signal-safe.
    static void CALLBACK on_signaled(PVOID param, BOOLEAN /*timer_fired*/) noexcept
    {
        const auto * e = static_cast<watch_entry *>(param);
        ::PostQueuedCompletionStatus(
            e->iocp, 0,
            reinterpret_cast<ULONG_PTR>(e->token),
            nullptr);
    }
};

// ── iocp_backend ──────────────────────────────────────────────────────────────

class iocp_backend : public backend {
public:
    iocp_backend()
    {
        // NumberOfConcurrentThreads = 1: only our event-loop thread calls
        // GetQueuedCompletionStatusEx, so there is no contention to manage.
        iocp_ = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 1);
        if (!iocp_)
            throw std::runtime_error(
                "iocp_backend: CreateIoCompletionPort failed (err="
                + std::to_string(::GetLastError()) + ")");
    }

    ~iocp_backend() override
    {
        // Unregister all waitable watches before destroying the IOCP handle.
        // UnregisterWaitEx with INVALID_HANDLE_VALUE blocks until any in-flight
        // thread-pool callback has returned, preventing use-after-free.
        {
            std::lock_guard lock(mutex_);
            for (auto & [handle, entry] : watches_) {
                if (entry->wait_reg) {
                    ::UnregisterWaitEx(entry->wait_reg, INVALID_HANDLE_VALUE);
                    entry->wait_reg = nullptr;
                }
            }
            watches_.clear();
        }
        ::CloseHandle(iocp_);
    }

    // ── add ───────────────────────────────────────────────────────────────────

    void add(native_handle_t handle, void * token, io_interest /*interest*/) override
    {
        // Attempt the IOCP path.  Succeeds for sockets and overlapped files.
        if (::CreateIoCompletionPort(handle, iocp_,
                reinterpret_cast<ULONG_PTR>(token), 0) != nullptr)
            return;

        const DWORD err = ::GetLastError();

        // ERROR_INVALID_PARAMETER: handle type does not support overlapped I/O.
        // ERROR_ACCESS_DENIED:     handle is already associated with an IOCP
        //                          (e.g. the same socket registered twice).
        if (err != ERROR_INVALID_PARAMETER && err != ERROR_ACCESS_DENIED)
            throw std::runtime_error(
                "iocp_backend::add: CreateIoCompletionPort failed (err="
                + std::to_string(err) + ")");

        // Fall back to the waitable path.
        _add_waitable(handle, token);
    }

    // ── modify ────────────────────────────────────────────────────────────────
    //
    // Waitable path: re-arms the one-shot NT thread-pool wait so the next
    // signal on the handle produces a new completion.  Call this after each
    // event to keep receiving notifications.
    //
    // IOCP path (sockets): no-op — the next overlapped operation issued by the
    // caller will produce the next completion automatically.

    void modify(native_handle_t handle, void * /*token*/, io_interest /*interest*/) override
    {
        std::lock_guard lock(mutex_);
        auto it = watches_.find(handle);
        if (it == watches_.end())
            return; // IOCP-capable handle — nothing to do

        _rearm(*it->second, handle);
    }

    // ── remove ────────────────────────────────────────────────────────────────
    //
    // Waitable path: cancels the NT thread-pool wait and removes the entry.
    // Blocks until any in-progress callback has returned.
    //
    // IOCP path: Windows provides no explicit deregistration.  Closing the
    // handle (caller's responsibility) implicitly removes it from the IOCP.
    //
    // Does NOT close the handle in either case.

    void remove(native_handle_t handle, void * /*token*/, io_interest /*interest*/) override
    {
        std::lock_guard lock(mutex_);
        auto it = watches_.find(handle);
        if (it == watches_.end())
            return; // IOCP-capable handle

        auto & entry = it->second;
        if (entry->wait_reg) {
            // Block until the callback (if currently executing) has returned.
            // Safe to hold mutex_ here: the callback only calls
            // PostQueuedCompletionStatus, which does not acquire mutex_.
            ::UnregisterWaitEx(entry->wait_reg, INVALID_HANDLE_VALUE);
            entry->wait_reg = nullptr;
        }
        watches_.erase(it);
    }

    // ── wake ──────────────────────────────────────────────────────────────────
    //
    // Completion key 0 is the wakeup sentinel.
    // All real tokens are reactor_handle pointers — guaranteed non-null.

    void wake() override
    {
        ::PostQueuedCompletionStatus(iocp_, 0, 0 /*sentinel*/, nullptr);
    }

    // ── wait ──────────────────────────────────────────────────────────────────
    //
    // Retrieves up to `capacity` completions in a single syscall.
    //
    // Returned backend_event:
    //   wakeup  — token = nullptr,         events = io_event::wakeup
    //   reactor — token = reactor_handle*, events = io_event::read

    std::size_t wait(backend_event * out, std::size_t capacity,
                     std::optional<duration_type> timeout) override
    {
        const DWORD timeout_ms = _to_timeout_ms(timeout);

        std::vector<OVERLAPPED_ENTRY> iocp_entries(capacity);
        ULONG removed = 0;

        const BOOL ok = ::GetQueuedCompletionStatusEx(
            iocp_,
            iocp_entries.data(),
            static_cast<ULONG>(capacity),
            &removed,
            timeout_ms,
            /*fAlertable=*/FALSE);

        if (!ok) {
            const DWORD err = ::GetLastError();
            if (err == WAIT_TIMEOUT || err == ERROR_ABANDONED_WAIT_0)
                return 0;
            throw std::runtime_error(
                "iocp_backend::wait: GetQueuedCompletionStatusEx failed (err="
                + std::to_string(err) + ")");
        }

        std::size_t count = 0;
        for (ULONG i = 0; i < removed; ++i) {
            const ULONG_PTR key = iocp_entries[i].lpCompletionKey;

            backend_event ev {};
            if (key == 0) {
                ev.events = io_event::wakeup;
            } else {
                ev.token  = reinterpret_cast<void *>(key);
                ev.events = io_event::read;

                // TODO (network module): inspect iocp_entries[i].lpOverlapped
                // to distinguish read vs write completions for overlapped sockets.
            }
            out[count++] = ev;
        }
        return count;
    }

private:
    // ── internal helpers ──────────────────────────────────────────────────────

    // Register a new entry for a non-IOCP handle.
    // Must NOT hold mutex_ on entry (acquires it after registration).
    void _add_waitable(native_handle_t handle, void * token)
    {
        auto entry = std::make_unique<watch_entry>();
        entry->iocp     = iocp_;
        entry->token    = token;
        entry->wait_reg = nullptr; // not armed yet — modify() does the first arm

        std::lock_guard lock(mutex_);
        watches_.emplace(handle, std::move(entry));
    }

    // Re-arm an existing one-shot wait.  Caller must hold mutex_.
    void _rearm(watch_entry & entry, native_handle_t handle)
    {
        if (entry.wait_reg) {
            // The one-shot fired (otherwise we would not be re-arming).
            // Release the internal wait handle; the callback has already returned.
            ::UnregisterWait(entry.wait_reg); // non-blocking, does not wait
            entry.wait_reg = nullptr;
        }
        _do_register_wait(entry, handle);
    }

    // Calls RegisterWaitForSingleObject.  No locking — called from
    // _add_waitable (before map insertion) and _rearm (lock already held).
    void _do_register_wait(watch_entry & entry, native_handle_t handle)
    {
        HANDLE wait_reg = nullptr;
        if (!::RegisterWaitForSingleObject(
                &wait_reg,
                handle,
                watch_entry::on_signaled,
                &entry,
                INFINITE,
                WT_EXECUTEONLYONCE)) {
            throw std::runtime_error(
                "iocp_backend: RegisterWaitForSingleObject failed (err="
                + std::to_string(::GetLastError()) + ")");
        }
        entry.wait_reg = wait_reg;
    }

    // Convert an optional duration to a DWORD millisecond timeout for the
    // Win32 wait functions.  Clamps to the legal [0, INFINITE-1] range.
    static DWORD _to_timeout_ms(std::optional<duration_type> timeout) noexcept
    {
        if (!timeout)
            return INFINITE;
        const auto ms = std::chrono::duration_cast<
            std::chrono::milliseconds>(*timeout).count();
        if (ms <= 0)
            return 0;
        // INFINITE (0xFFFFFFFF) is reserved; clamp to one below.
        if (ms >= static_cast<decltype(ms)>(INFINITE))
            return INFINITE - 1;
        return static_cast<DWORD>(ms);
    }

    // ── data ──────────────────────────────────────────────────────────────────

    HANDLE     iocp_;
    std::mutex mutex_;

    // Entries for waitable-path handles, keyed by native handle value.
    // IOCP-path handles (sockets) are not tracked here — the IOCP itself
    // keeps the association.
    std::unordered_map<native_handle_t, std::unique_ptr<watch_entry>> watches_;
};

} // namespace nx::asio

#endif // NX_CORE_ASIO_BACKEND_IOCP_INL
