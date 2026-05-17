// buffered_echo_demo — line-oriented TCP echo using read_accumulator.
//   Server echoes each line back with "[echo] " prefix.
//   Client sends three lines and collects the responses, then quits.

#include <nx/network/network.hpp>
#include <nx/core2/core2.hpp>
#include <nx/logging.hpp>

#include <string>
#include <string_view>
#include <vector>

int main()
{
    nx::logging::set_default_log_domain("buffered_echo");
    nx::core::local_thread main_thread("main");
    nx::core::loop         loop;

    using namespace nx::network;

    // ── Server ────────────────────────────────────────────────────────────────

    tcp::server server;

    nx::core::connect(&server, &tcp::server::new_connection, &server,
        [](std::shared_ptr<tcp::socket> conn) {
            // Each accepted connection gets its own accumulator (heap, conn owns it).
            auto * acc = new read_accumulator(conn.get());
            nx::core::connect(conn.get(), &tcp::socket::data_received, acc,
                [acc](nx::span<const char> data) { acc->feed(data); });

            nx::core::connect(acc, &read_accumulator::data_available, acc,
                [acc, conn]() {
                    while (auto r = acc->try_read_until('\n')) {
                        const std::string line = "[echo] "
                            + std::string(r->data(), r->size());
                        conn->write(line.data(), line.size());
                        acc->consume(r->size());
                    }
                });
        });

    server.listen(endpoint { ip_address::loopback_v4(), 19100 });
    nx_info("server listening on 127.0.0.1:19100");

    // ── Client ────────────────────────────────────────────────────────────────

    tcp::socket      client;
    read_accumulator client_acc(&client);
    std::vector<std::string> received;

    nx::core::connect(&client, &tcp::socket::data_received, &client_acc,
        [&client_acc](nx::span<const char> data) { client_acc.feed(data); });

    nx::core::connect(&client, &tcp::socket::connected, &client, [&]() {
        nx_info("client connected - sending lines");
        const char lines[] = "hello\nworld\nfoo\n";
        client.write(lines, sizeof(lines) - 1);
    });

    nx::core::connect(&client_acc, &read_accumulator::data_available, &client_acc, [&]() {
        while (auto r = client_acc.try_read_until('\n')) {
            received.emplace_back(r->data(), r->size() - 1);  // strip '\n'
            client_acc.consume(r->size());
            nx_info("got: '{}'", received.back());
        }
        if (received.size() == 3)
            loop.quit();
    });

    nx::core::connect(&client, &tcp::socket::error_occurred, &client,
        [&](nx::error e) {
            nx_critical("error: {}", e.what());
            loop.exit(1);
        });

    client.open();
    client.connect(endpoint { ip_address::loopback_v4(), 19100 })
        .or_else([](nx::error e) {
            nx_critical("connect failed: {}", e.what());
            std::exit(1);
        });

    return loop.exec();
}
