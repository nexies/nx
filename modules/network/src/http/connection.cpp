#include <nx/network/http/connection.hpp>

namespace nx::network::http {

connection::connection(std::shared_ptr<tcp::socket> sock, nx::core::object * parent)
    : nx::core::object(parent)
    , socket_(std::move(sock))
{
    acc_    = new read_accumulator(this);
    parser_ = new request_parser(acc_, this);

    nx::core::connect(socket_.get(), &tcp::socket::data_received, acc_,
        [acc = acc_](nx::span<const char> data) { acc->feed(data); });

    nx::core::connect(socket_.get(), &tcp::socket::disconnected, this,
        [this]() { NX_EMIT(closed) });

    nx::core::connect(socket_.get(), &tcp::socket::error_occurred, this,
        [this](nx::error) { _close(); });

    nx::core::connect(parser_, &request_parser::request_ready, this,
        [this](http::request req) { _on_request(std::move(req)); });

    nx::core::connect(parser_, &request_parser::parse_error, this,
        [this](nx::error) { _close(); });
}

// ── Private ───────────────────────────────────────────────────────────────────

void connection::_on_request(http::request req)
{
    const bool ka = req.keep_alive();

    responder_t respond = [this, ka](response r) {
        _send_response(std::move(r), ka);
    };

    NX_EMIT(request_received, std::move(req), std::move(respond))
}

void connection::_send_response(response r, bool keep_alive)
{
    const auto wire = r.serialize();
    socket_->write(wire.data(), wire.size());

    if (keep_alive)
        parser_->reset();
    else
        _close();
}

void connection::_close()
{
    socket_->disconnect();
    NX_EMIT(closed)
}

} // namespace nx::network::http
