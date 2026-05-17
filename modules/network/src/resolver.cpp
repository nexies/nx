#include <nx/network/resolver.hpp>
#include <nx/core2/thread/thread.hpp>

#if defined(NX_OS_WINDOWS)
#  include <winsock2.h>
#  include <ws2tcpip.h>
#else
#  include <sys/types.h>
#  include <sys/socket.h>
#  include <netdb.h>
#  include <netinet/in.h>
#endif

namespace nx::network {

namespace {

nx::result<std::vector<endpoint>>
do_getaddrinfo(const std::string & host, uint16_t port,
               std::optional<socket_family> hint) noexcept
{
    addrinfo hints {};
    hints.ai_socktype = SOCK_STREAM;    // suppress duplicate entries per socktype
    hints.ai_flags    = AI_NUMERICSERV;

    if (hint)
        hints.ai_family = (*hint == socket_family::ipv4) ? AF_INET : AF_INET6;
    else
        hints.ai_family = AF_UNSPEC;

    const std::string svc = std::to_string(port);
    addrinfo * res  = nullptr;
    const int  rc   = ::getaddrinfo(host.c_str(), svc.c_str(), &hints, &res);

    if (rc != 0)
        return nx::err::runtime_error(rc, "getaddrinfo failed");

    std::vector<endpoint> endpoints;

    for (const auto * p = res; p; p = p->ai_next) {
        endpoint ep;
        if (p->ai_family == AF_INET) {
            const auto * sa = reinterpret_cast<const sockaddr_in *>(p->ai_addr);
            ep.port    = ntohs(sa->sin_port);
            ep.address = ip_address::from_bytes(socket_family::ipv4,
                reinterpret_cast<const uint8_t *>(&sa->sin_addr), 4);
            endpoints.push_back(ep);
        } else if (p->ai_family == AF_INET6) {
            const auto * sa6 = reinterpret_cast<const sockaddr_in6 *>(p->ai_addr);
            ep.port    = ntohs(sa6->sin6_port);
            ep.address = ip_address::from_bytes(socket_family::ipv6,
                reinterpret_cast<const uint8_t *>(&sa6->sin6_addr), 16);
            endpoints.push_back(ep);
        }
    }

    ::freeaddrinfo(res);

    if (endpoints.empty())
        return nx::err::runtime_error("resolver: no addresses found");

    return endpoints;
}

} // anonymous namespace

// ── resolver ──────────────────────────────────────────────────────────────────

resolver::resolver(nx::core::object * parent)
    : nx::core::object(parent)
    , alive_(std::make_shared<std::atomic_bool>(true))
    , worker_(std::make_unique<nx::core::thread>("resolver-worker"))
{
    worker_->start();
}

resolver::~resolver()
{
    *alive_ = false;
    worker_->quit();
    worker_->wait();
}

nx::result<void> resolver::resolve(std::string_view             host,
                                    uint16_t                     port,
                                    std::optional<socket_family> hint)
{
    auto * t = get_thread();
    if (!t)
        return nx::err::invalid_state("resolver not assigned to a thread");

    auto alive    = alive_;
    auto host_str = std::string(host);

    worker_->post([this, alive, t, host_str = std::move(host_str), port, hint]() mutable {
        auto r = do_getaddrinfo(host_str, port, hint);

        t->post([this, alive, host_str = std::move(host_str), r = std::move(r)]() mutable {
            if (!*alive) return;

            if (r.is_error())
                NX_EMIT(error_occurred, r.error())
            else
                NX_EMIT(resolved, std::move(host_str), std::move(r.value()))
        });
    });

    return {};
}

nx::result<std::vector<endpoint>>
resolver::resolve_sync(std::string_view             host,
                        uint16_t                     port,
                        std::optional<socket_family> hint) noexcept
{
    return do_getaddrinfo(std::string(host), port, hint);
}

} // namespace nx::network
