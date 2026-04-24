#if !defined(NX_ASIO_SIGNAL_SET_WINDOWS_IMPL) && defined(NX_OS_WINDOWS)
#define NX_ASIO_SIGNAL_SET_WINDOWS_IMPL

#include <windows.h>
#include <csignal>
#include <mutex>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <memory>

#include "signal_set_impl.hpp"

namespace nx::asio {

class signal_set_windows : public signal_set::impl
{
    // -----------------------------------------------------------------------
    // Per-signal entry owned by this instance
    // -----------------------------------------------------------------------
    struct entry {
        HANDLE event_handle { nullptr };
        HANDLE wait_handle  { nullptr };
        int    signum       { 0 };
    };

    // -----------------------------------------------------------------------
    // Heap-allocated callback context – lives from add() until cleanup_entry().
    // Uses the base-class weak_ptr so enable_shared_from_this resolves cleanly.
    // -----------------------------------------------------------------------
    struct callback_ctx {
        std::weak_ptr<signal_set::impl> owner;
        int signum { 0 };
    };

    // -----------------------------------------------------------------------
    // Process-global state shared across all signal_set_windows instances
    // -----------------------------------------------------------------------
    struct global_t {
        std::mutex mutex;
        std::unordered_map<int, std::vector<HANDLE>> events_by_sig;
        int ctrl_refcount { 0 }; // SIGINT / SIGBREAK registrations
    };

    static global_t & global() {
        static global_t g;
        return g;
    }

    // -----------------------------------------------------------------------
    // Helpers
    // -----------------------------------------------------------------------
    static bool is_ctrl_signal(int signum) {
        if (signum == SIGINT) return true;
#ifdef SIGBREAK
        if (signum == SIGBREAK) return true;
#endif
        return false;
    }

    // Deliver to all event handles registered for this signal
    static void fire(int signum) {
        auto & g = global();
        std::lock_guard<std::mutex> lk(g.mutex);
        auto it = g.events_by_sig.find(signum);
        if (it == g.events_by_sig.end())
            return;
        for (HANDLE h : it->second)
            SetEvent(h);
    }

    // -----------------------------------------------------------------------
    // Signal / console handlers (process-global, all threads)
    // -----------------------------------------------------------------------
    static void c_signal_handler(int signum) {
        // Windows resets to SIG_DFL after delivery – re-install
        ::signal(signum, &signal_set_windows::c_signal_handler);
        fire(signum);
    }

    static BOOL WINAPI console_ctrl_handler(DWORD ctrl_type) {
        int signum = 0;
        switch (ctrl_type) {
        case CTRL_C_EVENT:     signum = SIGINT;   break;
#ifdef SIGBREAK
        case CTRL_BREAK_EVENT: signum = SIGBREAK; break;
#endif
        default: return FALSE;
        }
        fire(signum);
        return TRUE;
    }

    // -----------------------------------------------------------------------
    // Thread-pool callback – fires when a signal event is set
    // -----------------------------------------------------------------------
    static void CALLBACK wait_callback(PVOID param, BOOLEAN /*timer_or_wait*/) {
        auto * cc   = static_cast<callback_ctx*>(param);
        auto   base = cc->owner.lock();
        if (!base)
            return;

        auto * self   = static_cast<signal_set_windows*>(base.get());
        int    signum = cc->signum;

        handler_type h;
        {
            std::lock_guard<std::mutex> lk(self->mutex_);
            if (!self->handler_)
                return;
            h = std::move(self->handler_);
        }

        self->ctx_.post([h = std::move(h), signum]() mutable {
            h(signum);
        });
    }

    // -----------------------------------------------------------------------
    // Global registry management
    // -----------------------------------------------------------------------
    static void global_add(int signum, HANDLE ev) {
        auto & g = global();
        std::lock_guard<std::mutex> lk(g.mutex);
        auto & vec = g.events_by_sig[signum];
        vec.push_back(ev);
        if (is_ctrl_signal(signum)) {
            if (g.ctrl_refcount++ == 0)
                SetConsoleCtrlHandler(&signal_set_windows::console_ctrl_handler, TRUE);
        } else if (vec.size() == 1) {
            ::signal(signum, &signal_set_windows::c_signal_handler);
        }
    }

    static void global_remove(int signum, HANDLE ev) {
        auto & g = global();
        std::lock_guard<std::mutex> lk(g.mutex);
        auto it = g.events_by_sig.find(signum);
        if (it == g.events_by_sig.end())
            return;
        auto & vec = it->second;
        vec.erase(std::remove(vec.begin(), vec.end(), ev), vec.end());
        if (is_ctrl_signal(signum)) {
            if (--g.ctrl_refcount == 0)
                SetConsoleCtrlHandler(&signal_set_windows::console_ctrl_handler, FALSE);
        }
        if (vec.empty()) {
            g.events_by_sig.erase(it);
            if (!is_ctrl_signal(signum))
                ::signal(signum, SIG_DFL);
        }
    }

    // -----------------------------------------------------------------------
    // Cleanup a single entry.
    // Must be called WITHOUT holding mutex_ to avoid deadlocking with
    // wait_callback (which tries to acquire mutex_).
    // UnregisterWaitEx(INVALID_HANDLE_VALUE) blocks until all callbacks done.
    // -----------------------------------------------------------------------
    static void cleanup_entry(entry & e, callback_ctx * cc) {
        if (e.wait_handle) {
            UnregisterWaitEx(e.wait_handle, INVALID_HANDLE_VALUE);
            e.wait_handle = nullptr;
        }
        if (e.event_handle) {
            CloseHandle(e.event_handle);
            e.event_handle = nullptr;
        }
        delete cc;
    }

public:
    using handler_type = signal_set::handler_type;

    explicit signal_set_windows(io_context & ctx) : ctx_(ctx) {}

    ~signal_set_windows() override {
        std::vector<std::pair<entry, callback_ctx*>> todo;
        {
            std::lock_guard<std::mutex> lk(mutex_);
            handler_ = nullptr;
            todo.reserve(entries_.size());
            for (auto & [sig, e] : entries_) {
                auto it = callback_ctxs_.find(sig);
                todo.emplace_back(e, it != callback_ctxs_.end() ? it->second : nullptr);
            }
            entries_.clear();
            callback_ctxs_.clear();
        }
        for (auto & [e, cc] : todo) {
            global_remove(e.signum, e.event_handle);
            cleanup_entry(e, cc);
        }
    }

    void add(int signum) override {
        // Early-exit if already registered (no lock needed for the check alone;
        // signal_set is not expected to be called from multiple threads).
        {
            std::lock_guard<std::mutex> lk(mutex_);
            if (entries_.count(signum))
                return;
        }

        // Create resources outside the lock
        entry e;
        e.signum       = signum;
        e.event_handle = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (!e.event_handle)
            throw std::runtime_error("signal_set: CreateEvent failed");

        auto * cc = new callback_ctx{ weak_from_this(), signum };

        if (!RegisterWaitForSingleObject(&e.wait_handle, e.event_handle,
                &signal_set_windows::wait_callback, cc,
                INFINITE, WT_EXECUTEDEFAULT))
        {
            CloseHandle(e.event_handle);
            delete cc;
            throw std::runtime_error("signal_set: RegisterWaitForSingleObject failed");
        }

        global_add(signum, e.event_handle);

        std::lock_guard<std::mutex> lk(mutex_);
        entries_[signum]       = e;
        callback_ctxs_[signum] = cc;
    }

    void remove(int signum) override {
        entry         e{};
        callback_ctx* cc = nullptr;
        {
            std::lock_guard<std::mutex> lk(mutex_);
            auto it = entries_.find(signum);
            if (it == entries_.end())
                return;
            e = it->second;
            entries_.erase(it);
            auto cit = callback_ctxs_.find(signum);
            if (cit != callback_ctxs_.end()) {
                cc = cit->second;
                callback_ctxs_.erase(cit);
            }
        }
        global_remove(signum, e.event_handle);
        cleanup_entry(e, cc);
    }

    void clear() override {
        std::vector<std::pair<entry, callback_ctx*>> todo;
        {
            std::lock_guard<std::mutex> lk(mutex_);
            todo.reserve(entries_.size());
            for (auto & [sig, e] : entries_) {
                auto it = callback_ctxs_.find(sig);
                todo.emplace_back(e, it != callback_ctxs_.end() ? it->second : nullptr);
            }
            entries_.clear();
            callback_ctxs_.clear();
        }
        for (auto & [e, cc] : todo) {
            global_remove(e.signum, e.event_handle);
            cleanup_entry(e, cc);
        }
    }

    void async_wait(handler_type h) override {
        std::lock_guard<std::mutex> lk(mutex_);
        if (handler_)
            throw std::runtime_error("signal_set: already has a pending wait");
        handler_ = std::move(h);
    }

    std::size_t cancel() override {
        std::lock_guard<std::mutex> lk(mutex_);
        if (!handler_)
            return 0;
        handler_ = nullptr;
        return 1;
    }

private:
    io_context & ctx_;
    std::mutex   mutex_;
    handler_type handler_;
    std::unordered_map<int, entry>         entries_;
    std::unordered_map<int, callback_ctx*> callback_ctxs_;
};

} // namespace nx::asio

#endif // NX_ASIO_SIGNAL_SET_WINDOWS_IMPL
