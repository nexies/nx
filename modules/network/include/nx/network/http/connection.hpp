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

// ── http::connection<Stream> ──────────────────────────────────────────────────
//
// Manages the HTTP/1.1 protocol over a single stream (tcp::socket or
// tls::stream<tcp::socket>).  Stream must expose:
//   - signals: data_received(span<const char>), disconnected, error_occurred
//   - methods: write(const char*, size_t), disconnect()
//
// On each complete request, emits request_received with:
//   - the parsed request
//   - a responder_t callable — must be called exactly once with the response
//
// keep-alive: if the request signals keep-alive the parser resets; otherwise
// the stream is closed and `closed` is emitted.

template<typename Stream = tcp::socket>
class connection : public nx::core::object {
public:
    NX_OBJECT(connection)
    NX_DISABLE_COPY(connection)
    NX_DISABLE_MOVE(connection)

    using responder_t = std::function<void(response)>;

    explicit connection(std::shared_ptr<Stream> sock,
                        nx::core::object * parent = nullptr)
        : nx::core::object(parent)
        , socket_(std::move(sock))
    {
        acc_    = new read_accumulator(this);
        parser_ = new request_parser(acc_, this);

        nx::core::connect(socket_.get(), &Stream::data_received, acc_,
            [acc = acc_](nx::span<const char> data) { acc->feed(data); });

        nx::core::connect(socket_.get(), &Stream::disconnected, this,
            [this]() { NX_EMIT(closed) });

        nx::core::connect(socket_.get(), &Stream::error_occurred, this,
            [this](nx::error) { _close(); });

        nx::core::connect(parser_, &request_parser::request_ready, this,
            [this](http::request req) { _on_request(std::move(req)); });

        nx::core::connect(parser_, &request_parser::parse_error, this,
            [this](nx::error) { _close(); });
    }

    // ── Signals ───────────────────────────────────────────────────────────────

    NX_SIGNAL(request_received, http::request, responder_t)
    NX_SIGNAL(closed)

private:
    void _on_request(http::request req)
    {
        const bool ka = req.keep_alive();

        responder_t respond = [this, ka](response r) {
            _send_response(std::move(r), ka);
        };

        NX_EMIT(request_received, std::move(req), std::move(respond))
    }

    void _send_response(response r, bool keep_alive)
    {
        const auto wire = r.serialize();
        socket_->write(wire.data(), wire.size());

        if (keep_alive)
            parser_->reset();
        else
            _close();
    }

    void _close()
    {
        socket_->disconnect();
        NX_EMIT(closed)
    }

    std::shared_ptr<Stream> socket_;
    read_accumulator *      acc_    { nullptr };
    request_parser   *      parser_ { nullptr };
};

} // namespace nx::network::http
