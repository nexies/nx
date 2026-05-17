#pragma once

#include <nx/network/http/request.hpp>
#include <nx/network/http/response.hpp>

#include <functional>
#include <filesystem>
#include <string_view>

namespace nx::network::http {

// ── http::static_handler ──────────────────────────────────────────────────────
//
// Serves files from a root directory.  Typical usage with http::router:
//
//   http::static_handler files("/var/www/html");
//   router.any("/*", [&files](const request & req, auto respond) {
//       files.handle(req, std::move(respond));
//   });
//
// Security: path traversal (/../) is rejected with 403.

class static_handler {
public:
    using responder_t = std::function<void(response)>;

    explicit static_handler(std::filesystem::path root);

    void handle(const request & req, responder_t respond) const;

    static std::string_view mime_type(std::string_view extension) noexcept;

private:
    std::filesystem::path root_;
};

} // namespace nx::network::http
