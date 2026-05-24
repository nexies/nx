#include <nx/network/http/response_parser.hpp>

#include <algorithm>
#include <cctype>

namespace nx::network::http {

response_parser::response_parser(read_accumulator * acc, nx::core::object * parent)
    : nx::core::object(parent)
    , acc_(acc)
{
    nx::core::connect(acc_, &read_accumulator::data_available,
                      this, [this]() { on_data(); });
}

void response_parser::reset()
{
    state_          = state::status_line;
    body_remaining_ = 0;
    current_        = {};
    paused_         = false;
    on_data();
}

void response_parser::on_data()
{
    if (paused_) return;

    while (true) {
        switch (state_) {

        // ── Status line: "HTTP/1.x NNN Reason\r\n" ────────────────────────────
        case state::status_line: {
            auto line = acc_->try_read_until("\r\n");
            if (!line) return;

            std::string_view sv(line->data(), line->size() - 2);
            acc_->consume(line->size());

            // version
            const auto sp1 = sv.find(' ');
            if (sp1 == sv.npos) { _error("malformed status line"); return; }

            const auto ver = sv.substr(0, sp1);
            if (ver != "HTTP/1.1" && ver != "HTTP/1.0") {
                _error("unsupported HTTP version"); return;
            }

            sv.remove_prefix(sp1 + 1);

            // status code
            const auto sp2 = sv.find(' ');
            const auto code_sv = sp2 == sv.npos ? sv : sv.substr(0, sp2);
            try {
                current_.status_code = std::stoi(std::string(code_sv));
            } catch (...) {
                _error("invalid status code"); return;
            }

            // reason phrase (optional)
            if (sp2 != sv.npos)
                current_.reason = std::string(sv.substr(sp2 + 1));

            state_ = state::headers;
            break;
        }

        // ── Headers ───────────────────────────────────────────────────────────
        case state::headers: {
            auto line = acc_->try_read_until("\r\n");
            if (!line) return;

            std::string_view sv(line->data(), line->size() - 2);
            acc_->consume(line->size());

            if (sv.empty()) {
                const auto cl = current_.headers.find("content-length");
                const auto te = current_.headers.find("transfer-encoding");

                if (cl != current_.headers.end()) {
                    try { body_remaining_ = std::stoull(cl->second); }
                    catch (...) { _error("invalid content-length"); return; }
                    state_ = (body_remaining_ > 0) ? state::body : state::status_line;
                    if (body_remaining_ == 0) _complete();
                } else if (te != current_.headers.end()) {
                    // chunked or other TE: accumulate until connection closes
                    state_ = state::body_until_close;
                } else {
                    // No framing info: body ends when connection closes
                    state_ = state::body_until_close;
                }
                return;
            }

            const auto colon = sv.find(':');
            if (colon == sv.npos) { _error("malformed header"); return; }

            std::string name(sv.substr(0, colon));
            std::transform(name.begin(), name.end(), name.begin(),
                           [](unsigned char c){ return std::tolower(c); });

            std::string_view val = sv.substr(colon + 1);
            while (!val.empty() && val.front() == ' ')
                val.remove_prefix(1);

            current_.headers[std::move(name)] = std::string(val);
            break;
        }

        // ── Body (Content-Length) ─────────────────────────────────────────────
        case state::body: {
            auto chunk = acc_->try_read_exactly(body_remaining_);
            if (!chunk) return;

            current_.body.assign(chunk->begin(), chunk->end());
            acc_->consume(body_remaining_);
            _complete();
            return;
        }

        // ── Body (connection-close / chunked) ─────────────────────────────────
        case state::body_until_close: {
            // Drain any newly-arrived bytes into the response body.
            const auto buf = acc_->buffered();
            if (!buf.empty()) {
                current_.body.append(buf.data(), buf.size());
                acc_->consume(buf.size());
            }
            return; // wait for more data or finalize() on disconnect
        }
        }
    }
}

void response_parser::finalize()
{
    if (paused_) return;
    if (state_ == state::body_until_close)
        _complete();
}

void response_parser::_complete()
{
    paused_ = true;
    NX_EMIT(response_ready, std::move(current_))
    current_ = {};
}

void response_parser::_error(std::string_view msg)
{
    paused_ = true;
    NX_EMIT(parse_error, nx::err::runtime_error(std::string(msg)))
}

} // namespace nx::network::http
