// tcp_echo_demo — TCP echo on a single thread:
//   server listens on localhost:9876
//   client connects, sends a greeting, receives the echo, then quits.

#include <nx/network/network.hpp>
#include <nx/core2/core2.hpp>

#include <iostream>
#include <string_view>

int main()
{
    // Register the main OS thread with nx so objects created here get a thread context.
    nx::core::local_thread main_thread("main");
    nx::core::loop         loop;

    using namespace nx::network;

    // ── Server ────────────────────────────────────────────────────────────────

    tcp::server server;

    std::shared_ptr<tcp::socket> accepted_conn;

    nx::core::connect(&server, &tcp::server::new_connection, &server,
        [&](std::shared_ptr<tcp::socket> conn) {
            accepted_conn = conn;
            std::cout << "[server] accepted connection\n";

            nx::core::connect(conn.get(), &tcp::socket::ready_read, conn.get(),
                [&accepted_conn]() {
                    char buf[256];
                    auto n = accepted_conn->read(buf, sizeof(buf));
                    if (!n) {
                        std::cerr << "[server] read error: " << n.error().what() << "\n";
                        return;
                    }
                    std::cout << "[server] echoing " << *n << " bytes\n";
                    accepted_conn->write(buf, *n);
                });
        });

    nx::core::connect(&server, &tcp::server::error_occurred, &server,
        [](nx::error e) {
            std::cerr << "[server] error: " << e.what() << "\n";
        });

    auto r = server.listen(endpoint { ip_address::loopback_v4(), 9876 });
    if (!r) {
        std::cerr << "listen: " << r.error().what() << "\n";
        return 1;
    }
    std::cout << "[server] listening on 127.0.0.1:9876\n";

    // ── Client ────────────────────────────────────────────────────────────────

    tcp::socket client;

    auto open_r = client.open();
    if (!open_r) {
        std::cerr << "open: " << open_r.error().what() << "\n";
        return 1;
    }

    nx::core::connect(&client, &tcp::socket::connected, &client,
        [&client]() {
            std::cout << "[client] connected\n";
            const char msg[] = "Hello, nx::network!";
            client.write(msg, sizeof(msg) - 1);
        });

    nx::core::connect(&client, &tcp::socket::ready_read, &client,
        [&client, &loop]() {
            char buf[256];
            auto n = client.read(buf, sizeof(buf));
            if (n)
                std::cout << "[client] echo: " << std::string_view(buf, *n) << "\n";
            loop.quit();
        });

    nx::core::connect(&client, &tcp::socket::error_occurred, &client,
        [&loop](nx::error e) {
            std::cerr << "[client] error: " << e.what() << "\n";
            loop.exit(1);
        });

    auto conn_r = client.connect(endpoint { ip_address::loopback_v4(), 9876 });
    if (!conn_r) {
        std::cerr << "connect: " << conn_r.error().what() << "\n";
        return 1;
    }

    std::cout << "[demo] running event loop\n";
    return loop.exec();
}
