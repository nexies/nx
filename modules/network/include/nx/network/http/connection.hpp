#pragma once

#include <nx/network/http/request.hpp>
#include <nx/network/http/response.hpp>
#include <nx/network/http/request_parser.hpp>
#include <nx/network/buffer/read_accumulator.hpp>
#include <nx/network/tcp/socket.hpp>

#include <nx/common/helpers.hpp>
#include <nx/core2/object/object.hpp>

#include <functional>
#include <memory>

namespace nx::network::http {

// ── http::connection ──────────────────────────────────────────────────────────
//
// Manages the HTTP protocol over a single TCP connection.
// Owns the read_accumulator and request_parser.
//
// On each complete request, emits request_received with:
//   - the parsed request
//   - a responder_t callable — must be called exactly once with the response
//
// keep-alive: if the request signals keep-alive and the response is sent,
// the parser is reset and the connection waits for the next request.
// Otherwise the TCP socket is closed and `closed` is emitted.

class connection : public nx::core::object {
public:
    NX_OBJECT(connection)
    NX_DISABLE_COPY(connection)
    NX_DISABLE_MOVE(connection)

    using responder_t = std::function<void(response)>;

    explicit connection(std::shared_ptr<tcp::socket> sock,
                        nx::core::object * parent = nullptr);

    // ── Signals ───────────────────────────────────────────────────────────────

    NX_SIGNAL(request_received, http::request, responder_t)
    NX_SIGNAL(closed)

private:
    void _on_request(http::request req);
    void _send_response(response r, bool keep_alive);
    void _close();

    std::shared_ptr<tcp::socket> socket_;
    read_accumulator *           acc_;
    request_parser   *           parser_;
};

} // namespace nx::network::http
