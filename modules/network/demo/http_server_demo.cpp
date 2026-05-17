// http_server_demo — minimal HTTP/1.1 server.
//
// Handles:
//   GET  /hello        → 200 "Hello, World!"
//   GET  /echo?msg=... → 200 echoes the query string
//   *                  → 404

#include <nx/network/network.hpp>
#include <nx/core2/core2.hpp>
#include <nx/logging.hpp>

#include <string>

int main()
{
    nx::logging::set_default_log_domain("http_demo");

    nx::core::local_thread main_thread("main");
    nx::core::loop         loop;

    using namespace nx::network;

    http::server srv;

    nx::core::connect(&srv, &http::server::request_received, &srv,
        [&](http::request req, http::server::responder_t respond) {
            nx_info("{} {}", req.method, req.target);

            if (req.path() == "/hello") {
                respond(http::response::ok("Hello, World!\n"));

            } else if (req.path() == "/echo") {
                const auto msg = req.query();
                respond(http::response::ok(std::string(msg) + "\n"));

            } else if (req.path() == "/stop") {
                respond(http::response::ok("Bye!\n"));
                loop.exit(1);
                // post stop so the response is sent before loop exits
                // (will be connected via main_thread below)

            } else {
                respond(http::response::not_found());
            }
        });

    nx::core::connect(&srv, &http::server::error_occurred, &srv,
        [](nx::error e) { nx_critical("server error: {}", e.what()); });

    srv.listen(endpoint { ip_address::loopback_v4(), 8080 })
        .or_else([](nx::error e) {
            nx_critical("listen failed: {}", e.what());
            std::exit(1);
        });

    nx_info("HTTP server listening on http://127.0.0.1:8080");
    nx_info("Try: curl http://127.0.0.1:8080/hello");

    return loop.exec();
}
