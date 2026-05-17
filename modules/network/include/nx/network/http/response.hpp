#pragma once

#include <string>
#include <string_view>
#include <unordered_map>

namespace nx::network::http {

struct response {
    int         status_code { 200 };
    std::string reason      { "OK" };

    std::unordered_map<std::string, std::string> headers;
    std::string body;

    // ── Factories ─────────────────────────────────────────────────────────────

    static response ok            (std::string body = {},
                                   std::string_view content_type = "text/plain");
    static response not_found     (std::string body = "Not Found");
    static response forbidden     (std::string body = "Forbidden");
    static response bad_request   (std::string body = "Bad Request");
    static response internal_error(std::string body = "Internal Server Error");

    // ── Serialization ─────────────────────────────────────────────────────────

    std::string serialize() const;
};

} // namespace nx::network::http
