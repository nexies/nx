// Unit tests for http::request, http::response, and http::router.
// No sockets — all tests run synchronously.

#include <nx/network/http/request.hpp>
#include <nx/network/http/response.hpp>
#include <nx/network/http/router.hpp>

#include <catch2/catch_test_macros.hpp>

#include <string>
#include <string_view>

using namespace nx::network::http;

// ── Helpers ───────────────────────────────────────────────────────────────────

static request make_req(std::string method, std::string target,
                        std::string version = "1.1")
{
    request r;
    r.method  = std::move(method);
    r.target  = std::move(target);
    r.version = std::move(version);
    return r;
}

static response dispatched;
static auto capture = [](response res) { dispatched = std::move(res); };

// ── http::request: path / query ───────────────────────────────────────────────

TEST_CASE("request: path() strips query string", "[http][request]")
{
    auto r = make_req("GET", "/foo/bar?x=1&y=2");
    REQUIRE(r.path() == "/foo/bar");
}

TEST_CASE("request: path() returns full target when no query", "[http][request]")
{
    auto r = make_req("GET", "/foo/bar");
    REQUIRE(r.path() == "/foo/bar");
}

TEST_CASE("request: query() returns empty when no ?", "[http][request]")
{
    auto r = make_req("GET", "/path");
    REQUIRE(r.query().empty());
}

TEST_CASE("request: query() returns string after ?", "[http][request]")
{
    auto r = make_req("GET", "/path?a=1&b=2");
    REQUIRE(r.query() == "a=1&b=2");
}

// ── http::request: header ─────────────────────────────────────────────────────

TEST_CASE("request: header() returns empty for missing key", "[http][request]")
{
    auto r = make_req("GET", "/");
    REQUIRE(r.header("X-Missing").empty());
}

TEST_CASE("request: header() is case-insensitive lookup", "[http][request]")
{
    request r;
    r.headers["content-type"] = "text/plain";
    REQUIRE(r.header("Content-Type") == "text/plain");
    REQUIRE(r.header("CONTENT-TYPE") == "text/plain");
}

// ── http::request: keep_alive ─────────────────────────────────────────────────

TEST_CASE("request: HTTP/1.1 keep-alive by default", "[http][request]")
{
    auto r = make_req("GET", "/", "1.1");
    REQUIRE(r.keep_alive());
}

TEST_CASE("request: HTTP/1.1 Connection:close → not keep-alive", "[http][request]")
{
    auto r = make_req("GET", "/", "1.1");
    r.headers["connection"] = "close";
    REQUIRE_FALSE(r.keep_alive());
}

TEST_CASE("request: HTTP/1.0 not keep-alive by default", "[http][request]")
{
    auto r = make_req("GET", "/", "1.0");
    REQUIRE_FALSE(r.keep_alive());
}

TEST_CASE("request: HTTP/1.0 Connection:keep-alive → keep-alive", "[http][request]")
{
    auto r = make_req("GET", "/", "1.0");
    r.headers["connection"] = "keep-alive";
    REQUIRE(r.keep_alive());
}

// ── http::request: param ──────────────────────────────────────────────────────

TEST_CASE("request: param() returns empty for missing key", "[http][request]")
{
    auto r = make_req("GET", "/");
    REQUIRE(r.param("id").empty());
}

TEST_CASE("request: param() returns value when present", "[http][request]")
{
    auto r = make_req("GET", "/");
    r.params["id"] = "42";
    REQUIRE(r.param("id") == "42");
}

// ── http::request: query_params ───────────────────────────────────────────────

TEST_CASE("request: query_params() returns empty map for no query", "[http][request]")
{
    auto r = make_req("GET", "/path");
    REQUIRE(r.query_params().empty());
}

TEST_CASE("request: query_params() parses key=value", "[http][request]")
{
    auto r = make_req("GET", "/p?a=1&b=hello");
    const auto qp = r.query_params();
    REQUIRE(qp.size() == 2);
    REQUIRE(qp.at("a") == "1");
    REQUIRE(qp.at("b") == "hello");
}

TEST_CASE("request: query_params() URL-decodes percent encoding", "[http][request]")
{
    auto r = make_req("GET", "/p?msg=hello%20world");
    const auto qp = r.query_params();
    REQUIRE(qp.at("msg") == "hello world");
}

TEST_CASE("request: query_params() decodes + as space", "[http][request]")
{
    auto r = make_req("GET", "/p?msg=hello+world");
    REQUIRE(r.query_params().at("msg") == "hello world");
}

TEST_CASE("request: query_params() handles key with no value", "[http][request]")
{
    auto r = make_req("GET", "/p?flag");
    const auto qp = r.query_params();
    REQUIRE(qp.count("flag") == 1);
    REQUIRE(qp.at("flag").empty());
}

// ── http::request: body_sv ────────────────────────────────────────────────────

TEST_CASE("request: body_sv() returns view over body vector", "[http][request]")
{
    request r;
    std::string data = "hello body";
    r.body.assign(data.begin(), data.end());
    REQUIRE(r.body_sv() == data);
}

// ── http::response factories ──────────────────────────────────────────────────

TEST_CASE("response::ok() sets 200 with body and content-type", "[http][response]")
{
    const auto r = response::ok("hi", "text/plain");
    REQUIRE(r.status_code == 200);
    REQUIRE(r.body == "hi");
    REQUIRE(r.headers.count("Content-Type") == 1);
}

TEST_CASE("response::ok() with empty body has no Content-Type", "[http][response]")
{
    const auto r = response::ok();
    REQUIRE(r.status_code == 200);
    REQUIRE(r.headers.count("Content-Type") == 0);
}

TEST_CASE("response::not_found() sets 404", "[http][response]")
{
    REQUIRE(response::not_found().status_code == 404);
}

TEST_CASE("response::forbidden() sets 403", "[http][response]")
{
    REQUIRE(response::forbidden().status_code == 403);
}

TEST_CASE("response::bad_request() sets 400", "[http][response]")
{
    REQUIRE(response::bad_request().status_code == 400);
}

TEST_CASE("response::internal_error() sets 500", "[http][response]")
{
    REQUIRE(response::internal_error().status_code == 500);
}

// ── http::response::serialize ─────────────────────────────────────────────────

TEST_CASE("response::serialize() status line and headers", "[http][response]")
{
    const auto s = response::ok("body").serialize();
    REQUIRE(s.find("HTTP/1.1 200 OK\r\n") != std::string::npos);
    REQUIRE(s.find("\r\n\r\n") != std::string::npos);
    REQUIRE(s.rfind("body") == s.size() - 4);
}

TEST_CASE("response::serialize() injects Content-Length when absent", "[http][response]")
{
    response r;
    r.status_code = 200;
    r.reason      = "OK";
    r.body        = "hello";
    const auto s = r.serialize();
    REQUIRE(s.find("Content-Length: 5") != std::string::npos);
}

TEST_CASE("response::serialize() does not duplicate Content-Length", "[http][response]")
{
    response r;
    r.status_code = 200;
    r.reason      = "OK";
    r.headers["Content-Length"] = "5";
    r.body = "hello";
    const auto s = r.serialize();

    std::size_t first = s.find("ontent-Length");
    std::size_t second = s.find("ontent-Length", first + 1);
    REQUIRE(second == std::string::npos);
}

// ── http::router ──────────────────────────────────────────────────────────────

TEST_CASE("router: exact GET match calls handler", "[http][router]")
{
    router r;
    bool called = false;
    r.get("/hello", [&](const request &, auto respond) {
        called = true;
        respond(response::ok());
    });

    r.handle(make_req("GET", "/hello"), capture);
    REQUIRE(called);
}

TEST_CASE("router: method mismatch calls not_found", "[http][router]")
{
    router r;
    bool matched = false;
    r.get("/hello", [&](const request &, auto respond) { matched = true; respond(response::ok()); });

    int status = 0;
    r.handle(make_req("POST", "/hello"),
             [&](response res) { status = res.status_code; });
    REQUIRE_FALSE(matched);
    REQUIRE(status == 404);
}

TEST_CASE("router: unknown path returns 404 by default", "[http][router]")
{
    router r;
    int status = 0;
    r.handle(make_req("GET", "/nope"), [&](response res) { status = res.status_code; });
    REQUIRE(status == 404);
}

TEST_CASE("router: custom not_found handler is called", "[http][router]")
{
    router r;
    bool nf = false;
    r.not_found([&](const request &, auto respond) {
        nf = true;
        respond(response::not_found("custom"));
    });

    r.handle(make_req("GET", "/nowhere"), capture);
    REQUIRE(nf);
}

TEST_CASE("router: :param captured in req.params", "[http][router]")
{
    router r;
    std::string captured;
    r.get("/user/:id", [&](const request & req, auto respond) {
        captured = std::string(req.param("id"));
        respond(response::ok());
    });

    r.handle(make_req("GET", "/user/42"), capture);
    REQUIRE(captured == "42");
}

TEST_CASE("router: multiple :params captured", "[http][router]")
{
    router r;
    std::string a, b;
    r.get("/a/:x/b/:y", [&](const request & req, auto respond) {
        a = std::string(req.param("x"));
        b = std::string(req.param("y"));
        respond(response::ok());
    });

    r.handle(make_req("GET", "/a/foo/b/bar"), capture);
    REQUIRE(a == "foo");
    REQUIRE(b == "bar");
}

TEST_CASE("router: wildcard matches longer paths", "[http][router]")
{
    router r;
    bool called = false;
    r.any("/static/*", [&](const request &, auto respond) {
        called = true;
        respond(response::ok());
    });

    r.handle(make_req("GET", "/static/css/main.css"), capture);
    REQUIRE(called);
}

TEST_CASE("router: wildcard does not match shorter path", "[http][router]")
{
    router r;
    bool called = false;
    r.any("/files/*", [&](const request &, auto respond) {
        called = true;
        respond(response::ok());
    });

    // "/files" has 1 segment — wildcard route has 1 prefix segment + wildcard,
    // so 1 segment is not enough (must have at least 1 after the prefix).
    int status = 0;
    r.handle(make_req("GET", "/files"), [&](response res){ status = res.status_code; });
    REQUIRE_FALSE(called);
    REQUIRE(status == 404);
}

TEST_CASE("router: any() matches all HTTP methods", "[http][router]")
{
    router r;
    int count = 0;
    r.any("/ping", [&](const request &, auto respond) {
        ++count;
        respond(response::ok());
    });

    r.handle(make_req("GET",    "/ping"), capture);
    r.handle(make_req("POST",   "/ping"), capture);
    r.handle(make_req("DELETE", "/ping"), capture);
    REQUIRE(count == 3);
}

TEST_CASE("router: first matching route wins", "[http][router]")
{
    router r;
    int first = 0, second = 0;
    r.get("/x", [&](const request &, auto respond) { ++first;  respond(response::ok("first")); });
    r.get("/x", [&](const request &, auto respond) { ++second; respond(response::ok("second")); });

    r.handle(make_req("GET", "/x"), capture);
    REQUIRE(first  == 1);
    REQUIRE(second == 0);
}

TEST_CASE("router: query string ignored during matching", "[http][router]")
{
    router r;
    bool called = false;
    r.get("/search", [&](const request &, auto respond) {
        called = true;
        respond(response::ok());
    });

    r.handle(make_req("GET", "/search?q=test"), capture);
    REQUIRE(called);
}
