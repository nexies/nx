#pragma once

#include <nx/network/types.hpp>

#include <nx/common/helpers.hpp>
#include <nx/common/types/result.hpp>

#include <nx/core2/object/object.hpp>
#include <nx/core2/thread/thread.hpp>

#include <memory>

namespace nx::network::detail { class socket_impl; }
namespace nx::network::tcp   { class socket; }

namespace nx::network::tcp {

// ── tcp::server ───────────────────────────────────────────────────────────────
//
// TCP listening socket.  Accepts incoming connections and emits new_connection
// for each one with a ready-to-use tcp::socket.
//
// Usage:
//   auto srv = new tcp::server(parent);
//   connect(srv, &tcp::server::new_connection, handler, &Handler::on_conn);
//   srv->listen(endpoint { ip_address::any_v4(), 8080 });

class server : public nx::core::object {
public:
    NX_OBJECT(server)
    NX_DISABLE_COPY(server)
    NX_DISABLE_MOVE(server)

    explicit server(nx::core::object * parent = nullptr);
    ~server() override;

    // ── Control ───────────────────────────────────────────────────────────────

    nx::result<void> listen(const endpoint & local_ep,
                             socket_family    family  = socket_family::ipv4,
                             int              backlog = 128);
    void close();

    NX_NODISCARD bool is_listening() const noexcept;

    // ── Signals ───────────────────────────────────────────────────────────────

    // Emitted for each accepted connection.  The socket is already connected
    // and armed for reading; the receiver takes shared ownership.
    NX_SIGNAL(new_connection, std::shared_ptr<tcp::socket>)
    NX_SIGNAL(error_occurred, nx::error)

protected:
    void _on_thread_changed(nx::core::thread * old_t,
                             nx::core::thread * new_t) override;

private:
    void _on_io_event(nx::asio::io_event ev);
    void _arm();

    std::unique_ptr<detail::socket_impl> impl_;
    socket_family family_ { socket_family::ipv4 };
};

} // namespace nx::network::tcp
