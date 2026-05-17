#pragma once

#include <nx/network/http/connection.hpp>
#include <nx/network/tcp/server.hpp>
#include <nx/network/types.hpp>

#include <nx/common/helpers.hpp>
#include <nx/common/types/result.hpp>
#include <nx/core2/object/object.hpp>

#include <memory>
#include <unordered_set>

namespace nx::network::http {

// ── http::server ──────────────────────────────────────────────────────────────
//
// HTTP/1.1 server built on top of tcp::server.
//
// Usage:
//   http::server srv;
//   nx::core::connect(&srv, &http::server::request_received, &srv,
//       [](http::request req, http::server::responder_t respond) {
//           respond(http::response::ok("hello"));
//       });
//   srv.listen(endpoint { ip_address::loopback_v4(), 8080 });

class server : public nx::core::object {
public:
    NX_OBJECT(server)
    NX_DISABLE_COPY(server)
    NX_DISABLE_MOVE(server)

    using responder_t = connection::responder_t;

    explicit server(nx::core::object * parent = nullptr);
    ~server() override;

    // ── Control ───────────────────────────────────────────────────────────────

    nx::result<void> listen(const endpoint & ep, int backlog = 64);
    void             close();
    bool             is_listening() const noexcept;

    // ── Signals ───────────────────────────────────────────────────────────────

    NX_SIGNAL(request_received, http::request, responder_t)
    NX_SIGNAL(error_occurred,   nx::error)

private:
    void _on_new_connection(std::shared_ptr<tcp::socket> sock);

    tcp::server                                    tcp_server_;
    std::unordered_set<std::shared_ptr<connection>> connections_;
};

} // namespace nx::network::http
