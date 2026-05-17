#include <nx/network/http/request_parser.hpp>

#include <algorithm>
#include <cctype>

namespace nx::network::http {

request_parser::request_parser(read_accumulator * acc, nx::core::object * parent)
    : nx::core::object(parent)
    , acc_(acc)
{
    nx::core::connect(acc_, &read_accumulator::data_available,
                      this, [this]() { on_data(); });
}

// ── Public ────────────────────────────────────────────────────────────────────

void request_parser::reset()
{
    state_          = state::request_line;
    body_remaining_ = 0;
    current_        = {};
    paused_         = false;
    on_data(); // process data already sitting in the accumulator
}

// ── Slot ──────────────────────────────────────────────────────────────────────

void request_parser::on_data()
{
    if (paused_) return;

    while (true) {
        switch (state_) {

        // ── Request line: "METHOD /target HTTP/1.x\r\n" ───────────────────────
        case state::request_line: {
            auto line = acc_->try_read_until("\r\n");
            if (!line) return;

            std::string_view sv(line->data(), line->size() - 2);
            acc_->consume(line->size());

            const auto s1 = sv.find(' ');
            const auto s2 = sv.rfind(' ');
            if (s1 == sv.npos || s2 == s1) { _error("malformed request line"); return; }

            current_.method = std::string(sv.substr(0, s1));
            current_.target = std::string(sv.substr(s1 + 1, s2 - s1 - 1));

            const auto ver = sv.substr(s2 + 1);
            if      (ver == "HTTP/1.1") current_.version = "1.1";
            else if (ver == "HTTP/1.0") current_.version = "1.0";
            else { _error("unsupported HTTP version"); return; }

            state_ = state::headers;
            break;
        }

        // ── Headers: "Key: Value\r\n" … "\r\n" ───────────────────────────────
        case state::headers: {
            auto line = acc_->try_read_until("\r\n");
            if (!line) return;

            std::string_view sv(line->data(), line->size() - 2);
            acc_->consume(line->size());

            if (sv.empty()) {
                // Blank line = end of headers
                const auto cl = current_.content_length();
                if (cl > 0) {
                    body_remaining_ = cl;
                    state_ = state::body;
                } else {
                    _complete();
                    return;
                }
            } else {
                const auto colon = sv.find(':');
                if (colon == sv.npos) { _error("malformed header"); return; }

                std::string name(sv.substr(0, colon));
                std::transform(name.begin(), name.end(), name.begin(),
                               [](unsigned char c){ return std::tolower(c); });

                std::string_view val = sv.substr(colon + 1);
                while (!val.empty() && val.front() == ' ')
                    val.remove_prefix(1);

                current_.headers[std::move(name)] = std::string(val);
            }
            break;
        }

        // ── Body: exactly content_length bytes ────────────────────────────────
        case state::body: {
            auto chunk = acc_->try_read_exactly(body_remaining_);
            if (!chunk) return;

            current_.body.assign(chunk->begin(), chunk->end());
            acc_->consume(body_remaining_);
            _complete();
            return;
        }
        }
    }
}

// ── Private ───────────────────────────────────────────────────────────────────

void request_parser::_complete()
{
    paused_ = true;
    NX_EMIT(request_ready, std::move(current_))
    current_ = {};
}

void request_parser::_error(std::string_view msg)
{
    paused_ = true;
    NX_EMIT(parse_error, nx::err::runtime_error(std::string(msg)))
}

} // namespace nx::network::http
