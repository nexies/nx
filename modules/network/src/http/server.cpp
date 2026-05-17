#include <nx/network/http/server.hpp>

namespace nx::network::http {

server::server(nx::core::object * parent)
    : nx::core::object(parent)
    , tcp_server_(this)
{
    nx::core::connect(&tcp_server_, &tcp::server::new_connection, this,
        [this](std::shared_ptr<tcp::socket> sock) {
            _on_new_connection(std::move(sock));
        });

    nx::core::connect(&tcp_server_, &tcp::server::error_occurred, this,
        [this](nx::error e) { NX_EMIT(error_occurred, e) });
}

server::~server() = default;

// ── Control ───────────────────────────────────────────────────────────────────

nx::result<void> server::listen(const endpoint & ep, int backlog)
{
    return tcp_server_.listen(ep, socket_family::ipv4, backlog);
}

void server::close()
{
    tcp_server_.close();
    connections_.clear();
}

bool server::is_listening() const noexcept
{
    return tcp_server_.is_listening();
}

// ── Private ───────────────────────────────────────────────────────────────────

void server::_on_new_connection(std::shared_ptr<tcp::socket> sock)
{
    if (auto * t = get_thread())
        sock->move_to_thread(t);

    auto conn = std::make_shared<connection>(std::move(sock));

    if (auto * t = get_thread())
        conn->move_to_thread(t);

    connections_.insert(conn);

    nx::core::connect(conn.get(), &connection::request_received, this,
        [this](http::request req, responder_t resp) {
            NX_EMIT(request_received, std::move(req), std::move(resp))
        });

    // Defer removal to next loop iteration so the connection's signal
    // dispatch completes before the shared_ptr ref count drops to zero.
    nx::core::connect(conn.get(), &connection::closed, this,
        [this, weak = std::weak_ptr<connection>(conn)]() {
            if (auto * t = get_thread()) {
                t->post([this, weak]() {
                    if (auto c = weak.lock())
                        connections_.erase(c);
                });
            }
        });
}

} // namespace nx::network::http
