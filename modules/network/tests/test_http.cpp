// Integration tests for http::server + http::router over loopback TCP.

#include <nx/network/network.hpp>
#include <nx/core2/core2.hpp>

#include <catch2/catch_test_macros.hpp>

#include <string>
#include <string_view>
#include <vector>

using namespace nx::network;

// ── Helpers ───────────────────────────────────────────────────────────────────

// Sends raw bytes, collects everything until disconnect, returns raw response.
static std::string http_exchange(uint16_t port, std::string_view raw_request)
{
    nx::core::loop loop;

    tcp::socket client;
    std::string response;

    nx::core::connect(&client, &tcp::socket::connected, &client, [&]() {
        client.write(raw_request.data(), raw_request.size());
    });

    nx::core::connect(&client, &tcp::socket::data_received, &client,
        [&](nx::span<const char> data) {
            response.append(data.data(), data.size());
        });

    nx::core::connect(&client, &tcp::socket::disconnected, &client, [&]() {
        loop.quit();
    });

    nx::core::connect(&client, &tcp::socket::error_occurred, &client,
        [&](nx::error) { loop.exit(1); });

    client.open();
    client.connect(endpoint { ip_address::loopback_v4(), port });
    loop.exec();

    return response;
}

// Parse status code from "HTTP/1.1 NNN ..."
static int parse_status(const std::string & resp)
{
    const auto sp1 = resp.find(' ');
    if (sp1 == std::string::npos) return -1;
    const auto sp2 = resp.find(' ', sp1 + 1);
    const auto code_str = resp.substr(sp1 + 1, sp2 - sp1 - 1);
    return std::stoi(code_str);
}

// Extract body (after \r\n\r\n)
static std::string parse_body(const std::string & resp)
{
    const auto sep = resp.find("\r\n\r\n");
    if (sep == std::string::npos) return {};
    return resp.substr(sep + 4);
}

// Check if header line is present in response
static bool has_header(const std::string & resp, std::string_view name, std::string_view value)
{
    const auto sep = resp.find("\r\n\r\n");
    const auto headers = sep == std::string::npos ? resp : resp.substr(0, sep);
    const auto line = std::string(name) + ": " + std::string(value);
    return headers.find(line) != std::string::npos;
}

// ── Fixture: server with routes, listening on a fixed port ────────────────────

struct ServerFixture {
    nx::core::local_thread thread;
    http::router           router;
    http::server           srv;
    uint16_t               port;

    explicit ServerFixture(uint16_t p) : thread("srv"), port(p)
    {
        router.get("/hello", [](const http::request &, auto respond) {
            respond(http::response::ok("Hello, World!", "text/plain"));
        });

        router.get("/greet/:name", [](const http::request & req, auto respond) {
            respond(http::response::ok("Hello, " + std::string(req.param("name")) + "!"));
        });

        router.post("/echo", [](const http::request & req, auto respond) {
            respond(http::response::ok(std::string(req.body_sv()), "text/plain"));
        });

        router.get("/qs", [](const http::request & req, auto respond) {
            const auto qp = req.query_params();
            const auto it = qp.find("msg");
            respond(http::response::ok(it != qp.end() ? it->second : ""));
        });

        nx::core::connect(&srv, &http::server::request_received, &srv,
            [this](http::request req, http::server::responder_t respond) {
                router.handle(std::move(req), std::move(respond));
            });

        srv.listen(endpoint { ip_address::loopback_v4(), port })
            .or_else([](nx::error e) {
                FAIL("listen failed: " + std::string(e.what()));
            });
    }
};

// ── Tests ─────────────────────────────────────────────────────────────────────

TEST_CASE("http: GET /hello returns 200 with body", "[http][integration]")
{
    ServerFixture fix(19300);

    const auto resp = http_exchange(19300,
        "GET /hello HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Connection: close\r\n"
        "\r\n");

    REQUIRE(parse_status(resp) == 200);
    REQUIRE(parse_body(resp) == "Hello, World!");
}

TEST_CASE("http: GET unknown path returns 404", "[http][integration]")
{
    ServerFixture fix(19301);

    const auto resp = http_exchange(19301,
        "GET /not-found HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Connection: close\r\n"
        "\r\n");

    REQUIRE(parse_status(resp) == 404);
}

TEST_CASE("http: :param extracted from path", "[http][integration]")
{
    ServerFixture fix(19302);

    const auto resp = http_exchange(19302,
        "GET /greet/Alice HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Connection: close\r\n"
        "\r\n");

    REQUIRE(parse_status(resp) == 200);
    REQUIRE(parse_body(resp) == "Hello, Alice!");
}

TEST_CASE("http: POST body echoed back", "[http][integration]")
{
    ServerFixture fix(19303);

    const std::string body = "hello server";
    const std::string req =
        "POST /echo HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Content-Length: " + std::to_string(body.size()) + "\r\n"
        "Connection: close\r\n"
        "\r\n" + body;

    const auto resp = http_exchange(19303, req);

    REQUIRE(parse_status(resp) == 200);
    REQUIRE(parse_body(resp) == body);
}

TEST_CASE("http: query string param parsed from URL", "[http][integration]")
{
    ServerFixture fix(19304);

    const auto resp = http_exchange(19304,
        "GET /qs?msg=test123 HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Connection: close\r\n"
        "\r\n");

    REQUIRE(parse_status(resp) == 200);
    REQUIRE(parse_body(resp) == "test123");
}

TEST_CASE("http: response includes Content-Length header", "[http][integration]")
{
    ServerFixture fix(19305);

    const auto resp = http_exchange(19305,
        "GET /hello HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Connection: close\r\n"
        "\r\n");

    REQUIRE(has_header(resp, "Content-Length", std::to_string(parse_body(resp).size())));
}

TEST_CASE("http: keep-alive allows second request on same connection", "[http][integration]")
{
    ServerFixture fix(19306);

    nx::core::loop loop;

    tcp::socket  client;
    std::string  accumulated;
    int          responses = 0;

    nx::core::connect(&client, &tcp::socket::connected, &client, [&]() {
        const char req[] =
            "GET /hello HTTP/1.1\r\n"
            "Host: localhost\r\n"
            "Connection: keep-alive\r\n"
            "\r\n";
        client.write(req, sizeof(req) - 1);
        client.write(req, sizeof(req) - 1);
    });

    nx::core::connect(&client, &tcp::socket::data_received, &client,
        [&](nx::span<const char> data) {
            accumulated.append(data.data(), data.size());
            // Count completed HTTP responses by looking for the body marker
            std::size_t pos = 0;
            while (true) {
                const auto sep = accumulated.find("\r\n\r\n", pos);
                if (sep == std::string::npos) break;
                // Count Content-Length to skip body, then look for next response
                const auto cl_pos = accumulated.rfind("Content-Length: ", sep);
                if (cl_pos == std::string::npos) break;
                const auto cl_end = accumulated.find("\r\n", cl_pos);
                const std::size_t cl = std::stoull(accumulated.substr(cl_pos + 16, cl_end - cl_pos - 16));
                const std::size_t next = sep + 4 + cl;
                if (accumulated.size() < next) break;
                ++responses;
                pos = next;
                if (responses == 2) { loop.quit(); return; }
            }
        });

    nx::core::connect(&client, &tcp::socket::error_occurred, &client,
        [&](nx::error) { loop.exit(1); });

    client.open();
    client.connect(endpoint { ip_address::loopback_v4(), 19306 });
    loop.exec();

    REQUIRE(responses == 2);
}
