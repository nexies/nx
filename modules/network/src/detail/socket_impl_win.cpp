#include <nx/common/platform.hpp>
#if defined(NX_OS_WINDOWS)

#include "socket_impl.hpp"

#include <nx/asio/context/reactor_handle.hpp>

#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#  define NOMINMAX
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>

#include <cstring>
#include <stdexcept>

namespace nx::network::detail {

using io_event    = nx::asio::io_event;
using io_interest = nx::asio::io_interest;

// ── error helpers ─────────────────────────────────────────────────────────────

nx::err::runtime_error socket_impl::would_block_error()
{
    return nx::err::runtime_error(WSAEWOULDBLOCK, "would block");
}

bool socket_impl::is_would_block(const nx::error & e) noexcept
{
    const int v = e.value();
    return v == WSAEWOULDBLOCK || v == WSAEINPROGRESS;
}

static nx::err::runtime_error os_err(std::string_view ctx, int code = WSAGetLastError())
{
    return nx::err::runtime_error(code, ctx);
}

// ── sockaddr helpers ──────────────────────────────────────────────────────────

static void ep_to_sockaddr(const endpoint & ep,
                            sockaddr_storage & ss, int & len)
{
    std::memset(&ss, 0, sizeof(ss));
    if (ep.address.is_v4()) {
        auto & a4 = reinterpret_cast<sockaddr_in &>(ss);
        a4.sin_family = AF_INET;
        a4.sin_port   = htons(ep.port);
        std::memcpy(&a4.sin_addr, ep.address.bytes(), 4);
        len = sizeof(sockaddr_in);
    } else {
        auto & a6 = reinterpret_cast<sockaddr_in6 &>(ss);
        a6.sin6_family = AF_INET6;
        a6.sin6_port   = htons(ep.port);
        std::memcpy(&a6.sin6_addr, ep.address.bytes(), 16);
        len = sizeof(sockaddr_in6);
    }
}

static endpoint ep_from_sockaddr(const sockaddr_storage & ss)
{
    endpoint ep;
    if (ss.ss_family == AF_INET) {
        const auto & a4 = reinterpret_cast<const sockaddr_in &>(ss);
        ep.port = ntohs(a4.sin_port);
        ep.address = ip_address::from_bytes(socket_family::ipv4,
            reinterpret_cast<const uint8_t *>(&a4.sin_addr), 4);
    } else if (ss.ss_family == AF_INET6) {
        const auto & a6 = reinterpret_cast<const sockaddr_in6 &>(ss);
        ep.port = ntohs(a6.sin6_port);
        ep.address = ip_address::from_bytes(socket_family::ipv6,
            reinterpret_cast<const uint8_t *>(&a6.sin6_addr), 16);
    }
    return ep;
}

// ── socket_impl_win ───────────────────────────────────────────────────────────
//
// Uses WSAEventSelect so the socket's network events (FD_READ, FD_WRITE, ...)
// signal a Win32 manual-reset event.  That event handle is registered with the
// IOCP backend via the "waitable path" (RegisterWaitForSingleObject), which
// posts a completion to the IOCP when the event is set.
//
// All network events arrive through the reactor's read handler (the waitable
// path always delivers io_event::read).  We call WSAEnumNetworkEvents() inside
// the handler to determine the actual event type and translate it to io_event.

class socket_impl_win final : public socket_impl {
public:
    socket_impl_win() = default;

    // For accepted connections: socket is already created, event needs setup.
    explicit socket_impl_win(SOCKET s, WSAEVENT ev)
        : socket_(s), event_(ev)
    {}

    ~socket_impl_win() override { destroy(); }

    // ── Lifecycle ─────────────────────────────────────────────────────────────

    nx::result<void> create(socket_family family, socket_type type) override
    {
        const int af = (family == socket_family::ipv4) ? AF_INET : AF_INET6;
        const int st = (type == socket_type::tcp) ? SOCK_STREAM :
                       (type == socket_type::udp) ? SOCK_DGRAM  : SOCK_RAW;

        socket_ = ::socket(af, st, 0);
        if (socket_ == INVALID_SOCKET)
            return os_err("socket()");

        event_ = WSACreateEvent();
        if (event_ == WSA_INVALID_EVENT) {
            ::closesocket(socket_);
            socket_ = INVALID_SOCKET;
            return os_err("WSACreateEvent()");
        }

        _apply_wsa_event_select();
        return {};
    }

    void destroy() noexcept override
    {
        if (reactor_) {
            reactor_->uninstall();
            reactor_.reset();
        }
        if (event_ != WSA_INVALID_EVENT) {
            WSACloseEvent(event_);
            event_ = WSA_INVALID_EVENT;
        }
        if (socket_ != INVALID_SOCKET) {
            ::closesocket(socket_);
            socket_ = INVALID_SOCKET;
        }
        callback_ = nullptr;
    }

    void attach(nx::asio::io_context & ctx, event_cb cb) override
    {
        callback_ = std::move(cb);
        reactor_  = std::make_unique<nx::asio::reactor_handle>(ctx);
        // Register WSA event HANDLE — will take the waitable path in IOCP backend.
        reactor_->install(reinterpret_cast<nx::asio::native_handle_t>(event_),
                          io_interest::read);
        // On Windows the waitable path is armed via modify() (not install()).
        reactor_->modify(reinterpret_cast<nx::asio::native_handle_t>(event_),
                         io_interest::read);
        _arm_read_handler();
    }

    void detach() noexcept override
    {
        if (reactor_) {
            reactor_->uninstall();
            reactor_.reset();
        }
        callback_ = nullptr;
        write_armed_ = false;
    }

    // ── Async arming ──────────────────────────────────────────────────────────

    void arm_read() override
    {
        // The handler re-armed itself before calling callback, so nothing extra needed.
    }

    void arm_write() override
    {
        if (write_armed_) return;
        write_armed_ = true;
        net_mask_ |= FD_WRITE;
        _apply_wsa_event_select();
    }

    void disarm_write() override
    {
        if (!write_armed_) return;
        write_armed_ = false;
        net_mask_ &= ~FD_WRITE;
        _apply_wsa_event_select();
    }

    // ── Socket ops ────────────────────────────────────────────────────────────

    nx::result<void> bind(const endpoint & ep) override
    {
        sockaddr_storage ss {};
        int len = 0;
        ep_to_sockaddr(ep, ss, len);

        if (::bind(socket_, reinterpret_cast<sockaddr *>(&ss), len) == SOCKET_ERROR)
            return os_err("bind()");
        return {};
    }

    nx::result<void> listen(int backlog) override
    {
        net_mask_ |= FD_ACCEPT;
        net_mask_ &= ~FD_CONNECT;
        _apply_wsa_event_select();

        if (::listen(socket_, backlog) == SOCKET_ERROR)
            return os_err("listen()");
        return {};
    }

    nx::result<void> connect(const endpoint & ep) override
    {
        sockaddr_storage ss {};
        int len = 0;
        ep_to_sockaddr(ep, ss, len);

        // FD_CONNECT signals completion (or error) of the async connect.
        net_mask_ |= FD_CONNECT;
        _apply_wsa_event_select();

        if (::connect(socket_, reinterpret_cast<sockaddr *>(&ss), len) == 0)
            return {};

        const int err = WSAGetLastError();
        if (err == WSAEWOULDBLOCK || err == WSAEINPROGRESS)
            return {};

        return os_err("connect()");
    }

    // ── Options ───────────────────────────────────────────────────────────────

    nx::result<void> set_option_raw(opt_level level, opt_name name,
                                     const void * val, std::size_t len) override
    {
        auto [lv, nm] = _translate_opt(level, name);
        if (::setsockopt(socket_, lv, nm,
                         reinterpret_cast<const char *>(val),
                         static_cast<int>(len)) == SOCKET_ERROR)
            return os_err("setsockopt()");
        return {};
    }

    nx::result<void> get_option_raw(opt_level level, opt_name name,
                                     void * val, std::size_t & len) const override
    {
        auto [lv, nm] = _translate_opt(level, name);
        int optlen = static_cast<int>(len);
        if (::getsockopt(socket_, lv, nm,
                         reinterpret_cast<char *>(val), &optlen) == SOCKET_ERROR)
            return os_err("getsockopt()");
        len = static_cast<std::size_t>(optlen);
        return {};
    }

    // ── Non-blocking I/O ──────────────────────────────────────────────────────

    nx::result<std::size_t> read(char * buf, std::size_t len) override
    {
        const int n = ::recv(socket_, buf, static_cast<int>(len), 0);
        if (n == SOCKET_ERROR) {
            if (WSAGetLastError() == WSAEWOULDBLOCK) return would_block_error();
            return os_err("recv()");
        }
        return static_cast<std::size_t>(n);
    }

    nx::result<std::size_t> write(const char * buf, std::size_t len) override
    {
        const int n = ::send(socket_, buf, static_cast<int>(len), 0);
        if (n == SOCKET_ERROR) {
            if (WSAGetLastError() == WSAEWOULDBLOCK) return would_block_error();
            return os_err("send()");
        }
        return static_cast<std::size_t>(n);
    }

    nx::result<std::size_t> read_from(char * buf, std::size_t len, endpoint & from) override
    {
        sockaddr_storage ss {};
        int sslen = sizeof(ss);

        const int n = ::recvfrom(socket_, buf, static_cast<int>(len), 0,
                                  reinterpret_cast<sockaddr *>(&ss), &sslen);
        if (n == SOCKET_ERROR) {
            if (WSAGetLastError() == WSAEWOULDBLOCK) return would_block_error();
            return os_err("recvfrom()");
        }
        from = ep_from_sockaddr(ss);
        return static_cast<std::size_t>(n);
    }

    nx::result<std::size_t> write_to(const char * buf, std::size_t len,
                                      const endpoint & to) override
    {
        sockaddr_storage ss {};
        int sslen = 0;
        ep_to_sockaddr(to, ss, sslen);

        const int n = ::sendto(socket_, buf, static_cast<int>(len), 0,
                                reinterpret_cast<sockaddr *>(&ss), sslen);
        if (n == SOCKET_ERROR) {
            if (WSAGetLastError() == WSAEWOULDBLOCK) return would_block_error();
            return os_err("sendto()");
        }
        return static_cast<std::size_t>(n);
    }

    // ── Accept ────────────────────────────────────────────────────────────────

    nx::result<std::unique_ptr<socket_impl>> accept(endpoint & remote_ep) override
    {
        sockaddr_storage ss {};
        int sslen = sizeof(ss);

        const SOCKET client = ::accept(socket_,
                                        reinterpret_cast<sockaddr *>(&ss), &sslen);
        if (client == INVALID_SOCKET) {
            if (WSAGetLastError() == WSAEWOULDBLOCK) return would_block_error();
            return os_err("accept()");
        }

        remote_ep = ep_from_sockaddr(ss);

        // Create WSA event for the accepted socket and set up event selection.
        WSAEVENT ev = WSACreateEvent();
        if (ev == WSA_INVALID_EVENT) {
            ::closesocket(client);
            return os_err("WSACreateEvent() for accepted socket");
        }
        WSAEventSelect(client, ev,
                       FD_READ | FD_WRITE | FD_CLOSE);

        return std::unique_ptr<socket_impl>(new socket_impl_win(client, ev));
    }

    // ── Sync wait ─────────────────────────────────────────────────────────────

    nx::result<void> wait_readable(std::chrono::milliseconds timeout) override
    {
        const DWORD ms = (timeout.count() < 0)
            ? WSA_INFINITE
            : static_cast<DWORD>(timeout.count());

        const DWORD r = WSAWaitForMultipleEvents(1, &event_, FALSE, ms, FALSE);
        if (r == WSA_WAIT_TIMEOUT)
            return nx::err::runtime_error("wait_readable: timeout");
        if (r == WSA_WAIT_FAILED)
            return os_err("WSAWaitForMultipleEvents()");
        return {};
    }

    nx::result<void> wait_writable(std::chrono::milliseconds timeout) override
    {
        const DWORD ms = (timeout.count() < 0)
            ? WSA_INFINITE
            : static_cast<DWORD>(timeout.count());

        const DWORD r = WSAWaitForMultipleEvents(1, &event_, FALSE, ms, FALSE);
        if (r == WSA_WAIT_TIMEOUT)
            return nx::err::runtime_error("wait_writable: timeout");
        if (r == WSA_WAIT_FAILED)
            return os_err("WSAWaitForMultipleEvents()");

        // Reset the WSA event so any queued reactor completion delivers io_event::none
        // when the io_context drains — harmless but avoids a spurious write event.
        WSANETWORKEVENTS ne {};
        WSAEnumNetworkEvents(socket_, event_, &ne);
        return {};
    }

    int get_so_error() noexcept override
    {
        int err = 0;
        int len = sizeof(err);
        ::getsockopt(socket_, SOL_SOCKET, SO_ERROR,
                     reinterpret_cast<char *>(&err), &len);
        return err;
    }

    // ── State ─────────────────────────────────────────────────────────────────

    nx::asio::native_handle_t native_handle() const noexcept override
    {
        return reinterpret_cast<nx::asio::native_handle_t>(
            reinterpret_cast<void *>(static_cast<uintptr_t>(socket_)));
    }

    bool is_open() const noexcept override { return socket_ != INVALID_SOCKET; }

private:
    static std::pair<int, int> _translate_opt(opt_level level, opt_name name) noexcept
    {
        int lv = 0;
        switch (level) {
            case opt_level::socket: lv = SOL_SOCKET;   break;
            case opt_level::tcp:    lv = IPPROTO_TCP;  break;
            case opt_level::ip:     lv = IPPROTO_IP;   break;
            case opt_level::ipv6:   lv = IPPROTO_IPV6; break;
        }
        int nm = 0;
        switch (name) {
            case opt_name::reuse_address: nm = SO_REUSEADDR;  break;
            case opt_name::no_delay:      nm = TCP_NODELAY;   break;
            case opt_name::recv_buf_size: nm = SO_RCVBUF;     break;
            case opt_name::send_buf_size: nm = SO_SNDBUF;     break;
            case opt_name::broadcast:     nm = SO_BROADCAST;  break;
            case opt_name::ip_ttl:        nm = IP_TTL;        break;
            case opt_name::ipv6_only:     nm = IPV6_V6ONLY;   break;
        }
        return { lv, nm };
    }

    void _apply_wsa_event_select()
    {
        if (socket_ != INVALID_SOCKET && event_ != WSA_INVALID_EVENT)
            WSAEventSelect(socket_, event_, net_mask_);
    }

    // Translate WSANETWORKEVENTS → io_event
    static io_event _translate(const WSANETWORKEVENTS & ne) noexcept
    {
        io_event result = io_event::none;

        if (ne.lNetworkEvents & FD_READ)
            result = result | io_event::read;

        if (ne.lNetworkEvents & FD_ACCEPT)
            result = result | io_event::read;

        if (ne.lNetworkEvents & FD_WRITE)
            result = result | io_event::write;

        if (ne.lNetworkEvents & FD_CONNECT) {
            if (ne.iErrorCode[FD_CONNECT_BIT] != 0)
                result = result | io_event::error;
            else
                result = result | io_event::write; // connected → can write
        }

        if (ne.lNetworkEvents & FD_CLOSE)
            result = result | io_event::hangup;

        return result;
    }

    void _arm_read_handler()
    {
        if (!reactor_) return;
        reactor_->set_read_handler([this](nx::asio::backend_event &) {
            if (!callback_) return;

            // Re-arm the NT thread-pool wait before delivering the event,
            // so the next notification can be registered immediately.
            reactor_->modify(reinterpret_cast<nx::asio::native_handle_t>(event_),
                             io_interest::read);
            _arm_read_handler();

            // Determine what happened
            WSANETWORKEVENTS ne {};
            WSAEnumNetworkEvents(socket_, event_, &ne);

            const io_event ev = _translate(ne);
            if (ev != io_event::none)
                callback_(ev);
        });
    }

    SOCKET   socket_ { INVALID_SOCKET };
    WSAEVENT event_  { WSA_INVALID_EVENT };
    long     net_mask_ { FD_READ | FD_WRITE | FD_CLOSE };

    std::unique_ptr<nx::asio::reactor_handle> reactor_;
    event_cb callback_;
    bool     write_armed_ { false };
};

// ── Factory ───────────────────────────────────────────────────────────────────

std::unique_ptr<socket_impl> socket_impl::make()
{
    return std::make_unique<socket_impl_win>();
}

} // namespace nx::network::detail

#endif // NX_OS_WINDOWS
