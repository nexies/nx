// Integration tests for length_prefix_framer and delimiter_framer over loopback.

#include <nx/network/network.hpp>
#include <nx/network/framing/length_prefix_framer.hpp>
#include <nx/network/framing/delimiter_framer.hpp>
#include <nx/core2/core2.hpp>

#include <catch2/catch_test_macros.hpp>

#include <string>
#include <vector>

using namespace nx::network;
using namespace nx::network::framing;

static void wire_acc(tcp::socket & sock, read_accumulator & acc)
{
    nx::core::connect(&sock, &tcp::socket::data_received, &acc,
        [&acc](nx::span<const char> data) { acc.feed(data); });
}

// ── length_prefix_framer ──────────────────────────────────────────────────────

TEST_CASE("length_prefix_framer: receives multiple frames in order", "[framing][lpf]")
{
    nx::core::local_thread thread("test-main");
    nx::core::loop         loop;

    tcp::server          server;
    tcp::socket          client;
    read_accumulator     acc(&client);
    length_prefix_framer framer(&acc, &client);
    wire_acc(client, acc);

    std::vector<std::string> received;

    nx::core::connect(&server, &tcp::server::new_connection, &server,
        [](std::shared_ptr<tcp::socket> conn) {
            auto send_framed = [conn](std::string_view msg) {
                auto f = length_prefix_framer::pack(msg.data(), msg.size());
                conn->write(f.data(), f.size());
            };
            send_framed("hello");
            send_framed("world");
            send_framed("framing");
        });

    server.listen(endpoint { ip_address::loopback_v4(), 19200 });

    nx::core::connect(&framer, &length_prefix_framer::frame_received, &framer,
        [&](nx::span<const char> frame) {
            received.emplace_back(frame.data(), frame.size());
            if (received.size() == 3) loop.quit();
        });

    nx::core::connect(&client, &tcp::socket::error_occurred, &client,
        [&](nx::error) { loop.exit(1); });

    client.open();
    client.connect(endpoint { ip_address::loopback_v4(), 19200 });
    loop.exec();

    REQUIRE(received.size() == 3);
    REQUIRE(received[0] == "hello");
    REQUIRE(received[1] == "world");
    REQUIRE(received[2] == "framing");
}

TEST_CASE("length_prefix_framer: send / receive round-trip", "[framing][lpf]")
{
    nx::core::local_thread thread2("test-main2");
    nx::core::loop         loop;

    tcp::server          server;
    tcp::socket          client;
    read_accumulator     acc(&client);
    length_prefix_framer framer(&acc, &client);
    wire_acc(client, acc);

    std::vector<std::string> received;

    // Raw echo server.
    nx::core::connect(&server, &tcp::server::new_connection, &server,
        [](std::shared_ptr<tcp::socket> conn) {
            nx::core::connect(conn.get(), &tcp::socket::data_received, conn.get(),
                [conn](nx::span<const char> data) {
                    conn->write(data.data(), data.size());
                });
        });

    server.listen(endpoint { ip_address::loopback_v4(), 19201 });

    nx::core::connect(&client, &tcp::socket::connected, &client, [&]() {
        auto f1 = length_prefix_framer::pack("ping1", 5);
        auto f2 = length_prefix_framer::pack("ping2", 5);
        client.write(f1.data(), f1.size());
        client.write(f2.data(), f2.size());
    });

    nx::core::connect(&framer, &length_prefix_framer::frame_received, &framer,
        [&](nx::span<const char> frame) {
            received.emplace_back(frame.data(), frame.size());
            if (received.size() == 2) loop.quit();
        });

    nx::core::connect(&client, &tcp::socket::error_occurred, &client,
        [&](nx::error) { loop.exit(1); });

    client.open();
    client.connect(endpoint { ip_address::loopback_v4(), 19201 });
    loop.exec();

    REQUIRE(received.size() == 2);
    REQUIRE(received[0] == "ping1");
    REQUIRE(received[1] == "ping2");
}

// ── delimiter_framer ──────────────────────────────────────────────────────────

TEST_CASE("delimiter_framer: newline-delimited frames", "[framing][delim]")
{
    nx::core::local_thread thread3("test-main3");
    nx::core::loop         loop;

    tcp::server      server;
    tcp::socket      client;
    read_accumulator acc(&client);
    delimiter_framer framer(&acc, '\n', &client);
    wire_acc(client, acc);

    std::vector<std::string> received;

    nx::core::connect(&server, &tcp::server::new_connection, &server,
        [](std::shared_ptr<tcp::socket> conn) {
            const char msg[] = "alpha\nbeta\ngamma\n";
            conn->write(msg, sizeof(msg) - 1);
        });

    server.listen(endpoint { ip_address::loopback_v4(), 19210 });

    nx::core::connect(&framer, &delimiter_framer::frame_received, &framer,
        [&](nx::span<const char> frame) {
            received.emplace_back(frame.data(), frame.size());
            if (received.size() == 3) loop.quit();
        });

    nx::core::connect(&client, &tcp::socket::error_occurred, &client,
        [&](nx::error) { loop.exit(1); });

    client.open();
    client.connect(endpoint { ip_address::loopback_v4(), 19210 });
    loop.exec();

    REQUIRE(received.size() == 3);
    REQUIRE(received[0] == "alpha");
    REQUIRE(received[1] == "beta");
    REQUIRE(received[2] == "gamma");
}

TEST_CASE("delimiter_framer: CRLF delimiter", "[framing][delim]")
{
    nx::core::local_thread thread4("test-main4");
    nx::core::loop         loop;

    tcp::server      server;
    tcp::socket      client;
    read_accumulator acc(&client);
    delimiter_framer framer(&acc, std::string("\r\n"), &client);
    wire_acc(client, acc);

    std::vector<std::string> received;

    nx::core::connect(&server, &tcp::server::new_connection, &server,
        [](std::shared_ptr<tcp::socket> conn) {
            const char msg[] = "one\r\ntwo\r\nthree\r\n";
            conn->write(msg, sizeof(msg) - 1);
        });

    server.listen(endpoint { ip_address::loopback_v4(), 19211 });

    nx::core::connect(&framer, &delimiter_framer::frame_received, &framer,
        [&](nx::span<const char> frame) {
            received.emplace_back(frame.data(), frame.size());
            if (received.size() == 3) loop.quit();
        });

    nx::core::connect(&client, &tcp::socket::error_occurred, &client,
        [&](nx::error) { loop.exit(1); });

    client.open();
    client.connect(endpoint { ip_address::loopback_v4(), 19211 });
    loop.exec();

    REQUIRE(received.size() == 3);
    REQUIRE(received[0] == "one");
    REQUIRE(received[1] == "two");
    REQUIRE(received[2] == "three");
}
