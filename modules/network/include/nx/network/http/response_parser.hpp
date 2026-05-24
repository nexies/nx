#pragma once

#include <nx/network/http/response.hpp>
#include <nx/network/buffer/read_accumulator.hpp>

#include <nx/common/helpers.hpp>
#include <nx/common/types/result.hpp>
#include <nx/core2/object/object.hpp>

#include <cstddef>

namespace nx::network::http {

// ── response_parser ───────────────────────────────────────────────────────────
//
// HTTP/1.x response parser — client side counterpart of request_parser.
// Connect a read_accumulator and listen to response_ready.
//
// After response_ready fires the parser pauses; call reset() to parse
// the next response on a keep-alive connection.

class response_parser : public nx::core::object {
public:
    NX_OBJECT(response_parser)
    NX_DISABLE_COPY(response_parser)
    NX_DISABLE_MOVE(response_parser)

    explicit response_parser(read_accumulator * acc,
                              nx::core::object * parent = nullptr);

    void on_data();
    void reset();

    // Called when the connection closes.  If the response uses
    // connection-close framing (no Content-Length, no chunked), completes
    // the response with whatever body has been accumulated so far.
    void finalize();

    NX_SIGNAL(response_ready, http::response)
    NX_SIGNAL(parse_error,    nx::error)

private:
    enum class state { status_line, headers, body, body_until_close };

    void _complete();
    void _error(std::string_view msg);

    read_accumulator * acc_;
    state              state_          { state::status_line };
    bool               paused_         { false };
    std::size_t        body_remaining_ { 0 };
    response           current_;
};

} // namespace nx::network::http
