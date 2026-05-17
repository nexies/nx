// framing_demo — demonstrates length_prefix_framer and delimiter_framer.
//
// Part 1: length-prefix echo — client sends 3 frames, server echoes raw bytes back.
// Part 2: delimiter echo    — same, using newline delimiter.

#include <nx/network/network.hpp>
#include <nx/core2/core2.hpp>
#include <nx/logging.hpp>

#include <string_view>
#include <vector>

using namespace nx::network;
using namespace nx::network::framing;

// ── length-prefix part ────────────────────────────────────────────────────────

static int run_length_prefix(nx::core::local_thread & /*main_thread*/)
{
    nx::core::loop loop;

    tcp::server  srv;
    tcp::socket  sock;
    read_accumulator acc(&sock);
    length_prefix_framer framer(&acc, &sock);

    // Echo server: reflects raw bytes back unchanged.
    nx::core::connect(&srv, &tcp::server::new_connection, &srv,
        [](std::shared_ptr<tcp::socket> conn) {
            nx::core::connect(conn.get(), &tcp::socket::data_received, conn.get(),
                [conn](nx::span<const char> data) {
                    conn->write(data.data(), data.size());
                });
        });

    srv.listen(endpoint { ip_address::loopback_v4(), 19300 });

    std::vector<std::string> received;

    nx::core::connect(&sock, &tcp::socket::connected, &sock, [&]() {
        nx_info("[lpf] connected - sending frames");
        auto f1 = length_prefix_framer::pack("frame-one",   9);
        auto f2 = length_prefix_framer::pack("frame-two",   9);
        auto f3 = length_prefix_framer::pack("frame-three", 11);
        sock.write(f1.data(), f1.size());
        sock.write(f2.data(), f2.size());
        sock.write(f3.data(), f3.size());
    });

    nx::core::connect(&framer, &length_prefix_framer::frame_received, &framer,
        [&](nx::span<const char> f) {
            received.emplace_back(f.data(), f.size());
            nx_info("[lpf] received: '{}'", received.back());
            if (received.size() == 3) loop.quit();
        });

    nx::core::connect(&sock, &tcp::socket::error_occurred, &sock,
        [&](nx::error e) { nx_critical("[lpf] error: {}", e.what()); loop.exit(1); });

    // Connect data_received → accumulator
    nx::core::connect(&sock, &tcp::socket::data_received, &acc,
        [&acc](nx::span<const char> data) { acc.feed(data); });

    sock.open();
    sock.connect(endpoint { ip_address::loopback_v4(), 19300 })
        .or_else([](nx::error e) { nx_critical("connect: {}", e.what()); std::exit(1); });

    return loop.exec();
}

// ── delimiter part ────────────────────────────────────────────────────────────

static int run_delimiter(nx::core::local_thread & /*main_thread*/)
{
    nx::core::loop loop;

    tcp::server  srv;
    tcp::socket  sock;
    read_accumulator   acc(&sock);
    delimiter_framer   framer(&acc, '\n', &sock);

    nx::core::connect(&srv, &tcp::server::new_connection, &srv,
        [](std::shared_ptr<tcp::socket> conn) {
            nx::core::connect(conn.get(), &tcp::socket::data_received, conn.get(),
                [conn](nx::span<const char> data) {
                    conn->write(data.data(), data.size());
                });
        });

    srv.listen(endpoint { ip_address::loopback_v4(), 19301 });

    std::vector<std::string> received;

    nx::core::connect(&sock, &tcp::socket::connected, &sock, [&]() {
        nx_info("[delim] connected - sending frames");
        auto f1 = framer.pack("hello");
        auto f2 = framer.pack("world");
        auto f3 = framer.pack("done");
        sock.write(f1.data(), f1.size());
        sock.write(f2.data(), f2.size());
        sock.write(f3.data(), f3.size());
    });

    nx::core::connect(&framer, &delimiter_framer::frame_received, &framer,
        [&](nx::span<const char> f) {
            received.emplace_back(f.data(), f.size());
            nx_info("[delim] received: '{}'", received.back());
            if (received.size() == 3) loop.quit();
        });

    nx::core::connect(&sock, &tcp::socket::error_occurred, &sock,
        [&](nx::error e) { nx_critical("[delim] error: {}", e.what()); loop.exit(1); });

    nx::core::connect(&sock, &tcp::socket::data_received, &acc,
        [&acc](nx::span<const char> data) { acc.feed(data); });

    sock.open();
    sock.connect(endpoint { ip_address::loopback_v4(), 19301 })
        .or_else([](nx::error e) { nx_critical("connect: {}", e.what()); std::exit(1); });

    return loop.exec();
}

// ── main ──────────────────────────────────────────────────────────────────────

int main()
{
    nx::logging::set_default_log_domain("framing_demo");
    nx::core::local_thread main_thread("main");

    nx_info("=== length_prefix_framer ===");
    if (int r = run_length_prefix(main_thread); r != 0) return r;

    nx_info("=== delimiter_framer ===");
    return run_delimiter(main_thread);
}
