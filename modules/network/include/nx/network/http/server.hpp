#pragma once

#include <nx/network/http/connection.hpp>
#include <nx/network/tcp/server.hpp>
#include <nx/network/tcp/socket.hpp>
#include <nx/network/types.hpp>

#include <nx/common/helpers.hpp>
#include <nx/common/types/result.hpp>
#include <nx/core2/object/object.hpp>

#include <functional>
#include <memory>
#include <type_traits>
#include <unordered_set>

#include "nx/network/tls/tls.hpp"

namespace nx::network::http {

// ── http::server<Stream> ──────────────────────────────────────────────────────
//
// HTTP/1.1 server built on top of tcp::server.
//
// Plain HTTP (default):
//   http::server<> srv;   // or http::server<tcp::socket>
//   srv.listen(ep);
//
// HTTPS:
//   tls::context ctx(tls::context::role::server);
//   ctx.use_certificate_file("cert.pem");
//   ctx.use_private_key_file("key.pem");
//
//   http::server<tls::stream<tcp::socket>> srv(
//       [&ctx](std::shared_ptr<tcp::socket> s) {
//           return std::make_shared<tls::stream<tcp::socket>>(std::move(s), ctx);
//       });
//   srv.listen(ep);

template<typename Stream = tcp::socket>
class server : public nx::core::object {
public:
    NX_OBJECT(server)
    NX_DISABLE_COPY(server)
    NX_DISABLE_MOVE(server)

    using responder_t    = typename connection<Stream>::responder_t;
    using stream_factory = std::function<std::shared_ptr<Stream>(std::shared_ptr<tcp::socket>)>;

    // ── Plain TCP constructor (only available when Stream == tcp::socket) ──────

    template<typename S = Stream,
             typename   = std::enable_if_t<std::is_same_v<S, tcp::socket>>>
    explicit server(nx::core::object * parent = nullptr)
        : server(_identity_factory(), parent)
    {}

    // ── General constructor: caller supplies the stream factory ───────────────

    explicit server(stream_factory factory, nx::core::object * parent = nullptr)
        : nx::core::object(parent)
        , tcp_server_(this)
        , factory_(std::move(factory))
    {
        nx::core::connect(&tcp_server_, &tcp::server::new_connection, this,
            [this](std::shared_ptr<tcp::socket> sock) {
                _on_new_connection(std::move(sock));
            });

        nx::core::connect(&tcp_server_, &tcp::server::error_occurred, this,
            [this](nx::error e) { NX_EMIT(error_occurred, e) });
    }

    ~server() override = default;

    // ── Control ───────────────────────────────────────────────────────────────

    nx::result<void> listen(const endpoint & ep, int backlog = 64)
    {
        return tcp_server_.listen(ep, socket_family::ipv4, backlog);
    }

    void close()
    {
        tcp_server_.close();
        connections_.clear();
    }

    bool is_listening() const noexcept { return tcp_server_.is_listening(); }

    // ── Signals ───────────────────────────────────────────────────────────────

    NX_SIGNAL(request_received, http::request, responder_t)
    NX_SIGNAL(error_occurred,   nx::error)

private:
    static stream_factory _identity_factory()
    {
        return [](std::shared_ptr<tcp::socket> s) {
            return s;
        };
    }

    void _on_new_connection(std::shared_ptr<tcp::socket> raw_sock)
    {
        if (auto * t = get_thread())
            raw_sock->move_to_thread(t);

        auto stream = factory_(std::move(raw_sock));

        if (auto * t = get_thread())
            stream->move_to_thread(t);

        using conn_t = connection<Stream>;
        auto conn = std::make_shared<conn_t>(stream);

        if (auto * t = get_thread())
            conn->move_to_thread(t);

        connections_.insert(conn);

        nx::core::connect(conn.get(), &conn_t::request_received, this,
            [this](http::request req, responder_t resp) {
                NX_EMIT(request_received, std::move(req), std::move(resp))
            });

        nx::core::connect(conn.get(), &conn_t::closed, this,
            [this, weak = std::weak_ptr<conn_t>(conn)]() {
                if (auto * t = get_thread()) {
                    t->post([this, weak]() {
                        if (auto c = weak.lock())
                            connections_.erase(c);
                    });
                }
            });
    }

    tcp::server                                     tcp_server_;
    stream_factory                                  factory_;
    std::unordered_set<std::shared_ptr<connection<Stream>>> connections_;
};

} // namespace nx::network::http

namespace nx::network::https {
    using server = http::server<tls::stream<tcp::socket>>;
}