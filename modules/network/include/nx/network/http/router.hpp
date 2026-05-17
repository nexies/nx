#pragma once

#include <nx/network/http/request.hpp>
#include <nx/network/http/response.hpp>

#include <functional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace nx::network::http {

// ── http::router ──────────────────────────────────────────────────────────────
//
// Simple path router for http::server.
//
// Patterns:
//   "/path"          — exact match
//   "/path/:name"    — named capture: req.param("name")
//   "/path/*"        — wildcard: matches the rest of the path
//
// Usage:
//   http::router router;
//   router.get("/hello", [](const request & req, auto respond) {
//       respond(response::ok("hi"));
//   });
//   router.post("/data/:id", [](const request & req, auto respond) {
//       auto id = req.param("id");
//       respond(response::ok(std::string(id)));
//   });
//
//   // Wire to server:
//   nx::core::connect(&srv, &http::server::request_received, &srv,
//       [&router](http::request req, auto respond) {
//           router.handle(std::move(req), std::move(respond));
//       });

class router {
public:
    using responder_t = std::function<void(response)>;
    using handler_t   = std::function<void(const request &, responder_t)>;

    // ── Route registration ────────────────────────────────────────────────────

    router & get (std::string_view pattern, handler_t h);
    router & post(std::string_view pattern, handler_t h);
    router & put (std::string_view pattern, handler_t h);
    router & del (std::string_view pattern, handler_t h);
    router & any (std::string_view pattern, handler_t h); // all methods

    // Custom 404 handler.  Default returns response::not_found().
    router & not_found(handler_t h);

    // ── Dispatch ──────────────────────────────────────────────────────────────

    void handle(request req, responder_t respond) const;

private:
    struct route {
        std::string              method;       // "" = any
        std::vector<std::string> segments;     // pattern split on '/'
        std::vector<std::string> param_names;  // names of :param segments
        bool                     wildcard { false };
        handler_t                handler;
    };

    static route   _compile(std::string_view method, std::string_view pattern, handler_t h);
    static bool    _match  (const route & r, std::string_view method, std::string_view path,
                             std::unordered_map<std::string, std::string> & out_params);

    router & _add(std::string_view method, std::string_view pattern, handler_t h);

    std::vector<route> routes_;
    handler_t          not_found_handler_;
};

} // namespace nx::network::http
