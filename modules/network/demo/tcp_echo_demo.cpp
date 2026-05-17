// tcp_echo_demo — TCP echo on a single thread:
//   server listens on localhost:9876
//   client connects, sends a greeting, receives the echo, then quits.

#include <nx/network/network.hpp>
#include <nx/core2/core2.hpp>
#include <nx/logging.hpp>

#include <iostream>
#include <string_view>

int main()
{
    nx::logging::set_default_log_domain("demo_tcp");
    nx::core::local_thread main_thread("main");
    nx::core::loop         loop;

    const char echo_msg[] = "Hello, nx::network! :D :D :D :D :D :D";

    using namespace nx::network;

    // ── Server ────────────────────────────────────────────────────────────────

    tcp::server server;
    std::shared_ptr<tcp::socket> accepted_conn;

    nx::core::connect(&server, &tcp::server::new_connection, &server,
        [&](std::shared_ptr<tcp::socket> conn) {
            accepted_conn = conn;
            nx_info("server accepted connetion");

            nx::core::connect(conn.get(), &tcp::socket::data_received, conn.get(),
                [conn](nx::span<const char> data) {
                    nx_info("server echoing {} bytes", data.size());
                    conn->write(data.data(), data.size());
                });
        });


    auto server_endpoint = endpoint {ip_address::loopback_v4(), 9876};
    server.listen(server_endpoint)
        .or_else([](nx::error e) {
            nx_critical("server error: {}", e.what());
            std::exit(1);
        });

    nx_info("server listening on {}:{}", server_endpoint.address.to_string(), server_endpoint.port);

    // ── Client ────────────────────────────────────────────────────────────────

    tcp::socket client;
    client.open()
        .or_else([](nx::error e) {
            nx_critical("client error: {}", e.what());
            std::exit(1);
        });

    nx::core::connect(&client, &tcp::socket::connected, &client,
        [&client, &echo_msg]() {
            nx_info("client connected");
            client.write(echo_msg, sizeof(echo_msg) - 1);
        });

    nx::core::connect(&client, &tcp::socket::data_received, &client,
        [&loop](nx::span<const char> data) {
            nx_info("client echo: \"{}\"", std::string_view(data.data(), data.size()));
            loop.quit();
        });

    nx::core::connect(&client, &tcp::socket::error_occurred, &client,
        [&loop](nx::error e) {
            nx_critical("client error: {}", e.what());
            loop.exit(1);
        });

    client.connect(endpoint { ip_address::loopback_v4(), 9876 })
        .or_else([](nx::error e) {
            nx_critical("client error: {}", e.what());
            std::exit(2);
        });

    nx_info("running event loop");
    return loop.exec();
}
