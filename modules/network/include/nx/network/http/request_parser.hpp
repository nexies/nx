#pragma once

#include <nx/network/http/request.hpp>
#include <nx/network/buffer/read_accumulator.hpp>

#include <nx/common/helpers.hpp>
#include <nx/common/types/result.hpp>
#include <nx/core2/object/object.hpp>

#include <cstddef>

namespace nx::network::http {

// ── request_parser ────────────────────────────────────────────────────────────
//
// HTTP/1.x request parser.  Connect acc->data_available to on_data().
// Emits request_ready when a complete request (including body) is buffered.
//
// After request_ready fires the parser pauses — call reset() to continue
// parsing the next request (keep-alive) or construct a new parser.

class request_parser : public nx::core::object {
public:
    NX_OBJECT(request_parser)
    NX_DISABLE_COPY(request_parser)
    NX_DISABLE_MOVE(request_parser)

    explicit request_parser(read_accumulator * acc,
                             nx::core::object * parent = nullptr);

    // ── Slots ─────────────────────────────────────────────────────────────────

    void on_data();

    // Re-arm after request_ready to parse the next pipelined request.
    void reset();

    // ── Signals ───────────────────────────────────────────────────────────────

    NX_SIGNAL(request_ready, http::request)
    NX_SIGNAL(parse_error,   nx::error)

private:
    enum class state { request_line, headers, body };

    void _complete();
    void _error(std::string_view msg);

    read_accumulator * acc_;
    state              state_   { state::request_line };
    bool               paused_  { false };
    std::size_t        body_remaining_ { 0 };
    request            current_;
};

} // namespace nx::network::http
