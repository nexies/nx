// Integration tests for read_accumulator over loopback TCP.

#include <nx/network/network.hpp>
#include <nx/core2/core2.hpp>
#include <nx/logging.hpp>

#include <catch2/catch_test_macros.hpp>

#include <string>
#include <vector>

using namespace nx::network;

static void wire_acc(tcp::socket & sock, read_accumulator & acc)
{
    nx::core::connect(&sock, &tcp::socket::data_received, &acc,
        [&acc](nx::span<const char> data) { acc.feed(data); });
}

TEST_CASE("read_accumulator: try_read_exactly collects data", "[read_accumulator]")
{
    nx::core::local_thread thread("test-main");
    nx::core::loop         loop;

    tcp::server      server;
    tcp::socket      client;
    read_accumulator acc(&client);
    wire_acc(client, acc);

    std::string received;

    nx::core::connect(&server, &tcp::server::new_connection, &server,
        [](std::shared_ptr<tcp::socket> conn) {
            const char msg[] = "Hello, world!";
            conn->write(msg, sizeof(msg) - 1);
        });

    server.listen(endpoint { ip_address::loopback_v4(), 19001 });

    nx::core::connect(&acc, &read_accumulator::data_available, &acc, [&]() {
        if (auto r = acc.try_read_exactly(13)) {
            received = std::string(r->data(), r->size());
            acc.consume(r->size());
            loop.quit();
        }
    });

    nx::core::connect(&client, &tcp::socket::error_occurred, &client,
        [&](nx::error) { loop.exit(1); });

    client.open();
    client.connect(endpoint { ip_address::loopback_v4(), 19001 });
    loop.exec();

    REQUIRE(received == "Hello, world!");
}

TEST_CASE("read_accumulator: try_read_until newline", "[read_accumulator]")
{
    nx::core::local_thread thread("test-main2");
    nx::core::loop         loop;

    tcp::server      server;
    tcp::socket      client;
    read_accumulator acc(&client);
    wire_acc(client, acc);

    std::vector<std::string> lines;

    nx::core::connect(&server, &tcp::server::new_connection, &server,
        [](std::shared_ptr<tcp::socket> conn) {
            const char msg[] = "line1\nline2\nline3\n";
            conn->write(msg, sizeof(msg) - 1);
        });

    server.listen(endpoint { ip_address::loopback_v4(), 19002 });

    nx::core::connect(&acc, &read_accumulator::data_available, &acc, [&]() {
        while (auto r = acc.try_read_until('\n')) {
            lines.emplace_back(r->data(), r->size() - 1);
            acc.consume(r->size());
        }
        if (lines.size() == 3) loop.quit();
    });

    nx::core::connect(&client, &tcp::socket::error_occurred, &client,
        [&](nx::error) { loop.exit(1); });

    client.open();
    client.connect(endpoint { ip_address::loopback_v4(), 19002 });
    loop.exec();

    REQUIRE(lines.size() == 3);
    REQUIRE(lines[0] == "line1");
    REQUIRE(lines[1] == "line2");
    REQUIRE(lines[2] == "line3");
}
