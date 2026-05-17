// udp_echo_demo — UDP echo on a single thread:
//   server binds to 127.0.0.1:9877, receives datagrams, echoes them back
//   client binds to 127.0.0.1:9878, sends a datagram, receives the echo, quits.
//
//   Demonstrates:
//     recv_from / send_to  — explicit datagram I/O with sender info
//     and_then             — chain open() → bind() without intermediate checks
//     or_else(void)        — fire-and-forget error logging

#include <nx/network/network.hpp>
#include <nx/core2/core2.hpp>
#include <nx/logging.hpp>

#include <iostream>
#include <string_view>

int main()
{
    nx::core::local_thread main_thread("main");
    nx::core::loop         loop;

    using namespace nx::network;

    char srv_buf[512];
    char cli_buf[512];

    // ── Server ────────────────────────────────────────────────────────────────

    udp::socket server;

    // and_then: open succeeds  →  bind.  or_else(void): log + abort on any failure.
    server.open()
        .and_then([&] { return server.bind(endpoint { ip_address::loopback_v4(), 9877 }); })
        .or_else([](nx::error e) {
            nx_critical("server setup failed: {}", e.what());
            std::exit(1);
        });

    nx_info("server listening on 127.0.0.1:9877");

    nx::core::connect(&server, &udp::socket::ready_read, &server,
        [&server, &srv_buf]() {
            endpoint from;
            server.recv_from(srv_buf, sizeof(srv_buf), from)
                .and_then([&](std::size_t n) {
                    nx_info("server received {} bytes from {}", n, from.to_string());
                    return server.send_to(srv_buf, n, from);
                })
                .or_else([](nx::error e) {
                    nx_critical("server recv/send failed: {}", e.what());
                });
        });

    // ── Client ────────────────────────────────────────────────────────────────

    udp::socket client;

    client.open()
        .and_then([&] { return client.bind(endpoint { ip_address::loopback_v4(), 9878 }); })
        .or_else([](nx::error e) {
            nx_critical("client setup failed: {}", e.what());
            std::exit(1);
        });

    nx::core::connect(&client, &udp::socket::ready_read, &client,
        [&client, &cli_buf, &loop]() {
            endpoint from;
            client.recv_from(cli_buf, sizeof(cli_buf), from)
                .map([&cli_buf](std::size_t n) { return std::string_view(cli_buf, n); })
                .map([&from, &loop](std::string_view sv) {
                    nx_info("client echo from {}: {}", from.to_string(), sv);
                    loop.quit();
                })
                .or_else([](nx::error e) {
                    nx_critical("client recv failed: {}", e.what());
                });
        });

    // Send first datagram — no connect() needed, send_to specifies destination directly.
    const char msg[] = "Hello over UDP!";
    client.send_to(msg, sizeof(msg) - 1, endpoint { ip_address::loopback_v4(), 9877 })
        .or_else([](nx::error e) {
            nx_critical("client send failed: {}", e.what());
            std::exit(1);
        });

    nx_info("event loop running");
    return loop.exec();
}
