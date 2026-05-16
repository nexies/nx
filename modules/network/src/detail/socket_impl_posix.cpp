#include <nx/common/platform.hpp>
#if defined(NX_POSIX)

#include "socket_impl.hpp"

#include <nx/asio/context/reactor_handle.hpp>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <cerrno>
#include <cstring>

namespace nx::network::detail {

using io_event    = nx::asio::io_event;
using io_interest = nx::asio::io_interest;

// ── error helpers ─────────────────────────────────────────────────────────────

nx::error socket_impl::would_block_error()
{
    return nx::err::runtime_error(EAGAIN, "would block");
}

bool socket_impl::is_would_block(const nx::error & e) noexcept
{
    return e.code().value() == EAGAIN || e.code().value() == EWOULDBLOCK;
}

static nx::error os_err(std::string_view ctx, int code = errno)
{
    return nx::err::runtime_error(code, ctx);
}

// ── sockaddr helpers ──────────────────────────────────────────────────────────

static void ep_to_sockaddr(const endpoint & ep,
                            sockaddr_storage & ss, socklen_t & len)
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

// ── socket_impl_posix ─────────────────────────────────────────────────────────

class socket_impl_posix final : public socket_impl {
public:
    socket_impl_posix() = default;

    explicit socket_impl_posix(int existing_fd)
        : fd_(existing_fd)
    {}

    ~socket_impl_posix() override { destroy(); }

    // ── Lifecycle ─────────────────────────────────────────────────────────────

    nx::result<void> create(socket_family family, socket_type type) override
    {
        const int af = (family == socket_family::ipv4) ? AF_INET : AF_INET6;
        const int st = (type == socket_type::tcp) ? SOCK_STREAM :
                       (type == socket_type::udp) ? SOCK_DGRAM  : SOCK_RAW;

        fd_ = ::socket(af, st | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
        if (fd_ < 0)
            return os_err("socket()");
        return {};
    }

    void destroy() noexcept override
    {
        if (fd_ >= 0) {
            ::close(fd_);
            fd_ = -1;
        }
    }

    void attach(nx::asio::io_context & ctx, event_cb cb) override
    {
        callback_ = std::move(cb);
        reactor_  = std::make_unique<nx::asio::reactor_handle>(ctx);
        reactor_->install(fd_, io_interest::read);
        _arm_read_handler();
    }

    void detach() noexcept override
    {
        if (reactor_) {
            reactor_->uninstall();
            reactor_.reset();
        }
        callback_ = nullptr;
        writing_  = false;
    }

    // ── Async arming ──────────────────────────────────────────────────────────

    void arm_read() override   { _arm_read_handler(); }

    void arm_write() override
    {
        if (!reactor_ || writing_) return;
        writing_ = true;
        reactor_->modify(fd_, io_interest::read | io_interest::write);
        _arm_write_handler();
    }

    void disarm_write() override
    {
        if (!reactor_ || !writing_) return;
        writing_ = false;
        reactor_->modify(fd_, io_interest::read);
    }

    // ── Socket ops ────────────────────────────────────────────────────────────

    nx::result<void> bind(const endpoint & ep) override
    {
        sockaddr_storage ss {};
        socklen_t len = 0;
        ep_to_sockaddr(ep, ss, len);

        if (::bind(fd_, reinterpret_cast<sockaddr *>(&ss), len) != 0)
            return os_err("bind()");
        return {};
    }

    nx::result<void> listen(int backlog) override
    {
        if (::listen(fd_, backlog) != 0)
            return os_err("listen()");
        return {};
    }

    nx::result<void> connect(const endpoint & ep) override
    {
        sockaddr_storage ss {};
        socklen_t len = 0;
        ep_to_sockaddr(ep, ss, len);

        if (::connect(fd_, reinterpret_cast<sockaddr *>(&ss), len) == 0)
            return {};

        if (errno == EINPROGRESS)
            return {};  // async connect in progress — wait for write event

        return os_err("connect()");
    }

    // ── Options ───────────────────────────────────────────────────────────────

    nx::result<void> set_reuse_address(bool enable) override
    {
        const int v = enable ? 1 : 0;
        if (::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &v, sizeof(v)) != 0)
            return os_err("setsockopt(SO_REUSEADDR)");
        return {};
    }

    nx::result<void> set_no_delay(bool enable) override
    {
        const int v = enable ? 1 : 0;
        if (::setsockopt(fd_, IPPROTO_TCP, TCP_NODELAY, &v, sizeof(v)) != 0)
            return os_err("setsockopt(TCP_NODELAY)");
        return {};
    }

    // ── Non-blocking I/O ──────────────────────────────────────────────────────

    nx::result<std::size_t> read(char * buf, std::size_t len) override
    {
        const auto n = ::recv(fd_, buf, len, 0);
        if (n < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                return would_block_error();
            return os_err("recv()");
        }
        return static_cast<std::size_t>(n);
    }

    nx::result<std::size_t> write(const char * buf, std::size_t len) override
    {
        const auto n = ::send(fd_, buf, len, MSG_NOSIGNAL);
        if (n < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                return would_block_error();
            return os_err("send()");
        }
        return static_cast<std::size_t>(n);
    }

    nx::result<std::size_t> read_from(char * buf, std::size_t len, endpoint & from) override
    {
        sockaddr_storage ss {};
        socklen_t sslen = sizeof(ss);

        const auto n = ::recvfrom(fd_, buf, len, 0,
                                   reinterpret_cast<sockaddr *>(&ss), &sslen);
        if (n < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                return would_block_error();
            return os_err("recvfrom()");
        }
        from = ep_from_sockaddr(ss);
        return static_cast<std::size_t>(n);
    }

    nx::result<std::size_t> write_to(const char * buf, std::size_t len,
                                      const endpoint & to) override
    {
        sockaddr_storage ss {};
        socklen_t sslen = 0;
        ep_to_sockaddr(to, ss, sslen);

        const auto n = ::sendto(fd_, buf, len, MSG_NOSIGNAL,
                                 reinterpret_cast<sockaddr *>(&ss), sslen);
        if (n < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                return would_block_error();
            return os_err("sendto()");
        }
        return static_cast<std::size_t>(n);
    }

    // ── Accept ────────────────────────────────────────────────────────────────

    nx::result<std::unique_ptr<socket_impl>> accept(endpoint & remote_ep) override
    {
        sockaddr_storage ss {};
        socklen_t sslen = sizeof(ss);

        const int client_fd = ::accept4(fd_,
                                         reinterpret_cast<sockaddr *>(&ss), &sslen,
                                         SOCK_NONBLOCK | SOCK_CLOEXEC);
        if (client_fd < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                return would_block_error();
            return os_err("accept4()");
        }

        remote_ep = ep_from_sockaddr(ss);
        return std::unique_ptr<socket_impl>(new socket_impl_posix(client_fd));
    }

    // ── Sync wait ─────────────────────────────────────────────────────────────

    nx::result<void> wait_readable(std::chrono::milliseconds timeout) override
    {
        pollfd pfd { fd_, POLLIN, 0 };
        const int ms = (timeout.count() < 0) ? -1 : static_cast<int>(timeout.count());
        const int r  = ::poll(&pfd, 1, ms);
        if (r < 0)  return os_err("poll()");
        if (r == 0) return nx::err::runtime_error("wait_readable: timeout");
        return {};
    }

    // ── State ─────────────────────────────────────────────────────────────────

    nx::asio::native_handle_t native_handle() const noexcept override { return fd_; }
    bool                      is_open()       const noexcept override { return fd_ >= 0; }

private:
    void _arm_read_handler()
    {
        if (!reactor_) return;
        reactor_->set_read_handler([this](nx::asio::backend_event & ev) {
            if (!callback_) return;
            io_event result = io_event::none;
            if ((ev.events & io_event::read)   != io_event::none) result = result | io_event::read;
            if ((ev.events & io_event::error)  != io_event::none) result = result | io_event::error;
            if ((ev.events & io_event::hangup) != io_event::none) result = result | io_event::hangup;
            callback_(result);
        });
    }

    void _arm_write_handler()
    {
        if (!reactor_) return;
        reactor_->set_write_handler([this](nx::asio::backend_event & ev) {
            if (callback_) callback_(io_event::write);
        });
    }

    int fd_ { -1 };
    std::unique_ptr<nx::asio::reactor_handle> reactor_;
    event_cb callback_;
    bool writing_ { false };
};

// ── Factory ───────────────────────────────────────────────────────────────────

std::unique_ptr<socket_impl> socket_impl::make()
{
    return std::make_unique<socket_impl_posix>();
}

} // namespace nx::network::detail

#endif // NX_POSIX
