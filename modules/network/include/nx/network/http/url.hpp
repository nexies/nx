#pragma once

#include <nx/common/types/result.hpp>

#include <cstdint>
#include <string>
#include <string_view>

namespace nx::network::http {

struct url {
    std::string scheme;   // "http" or "https"
    std::string host;
    uint16_t    port  { 0 };  // 0 = use default for scheme
    std::string path;         // always starts with "/"
    std::string query;        // without leading "?"

    // Parses "scheme://host[:port][/path][?query]".
    static nx::result<url> parse(std::string_view raw);

    bool     is_secure()     const noexcept { return scheme == "https"; }
    uint16_t default_port()  const noexcept { return is_secure() ? 443 : 80; }
    uint16_t effective_port() const noexcept { return port ? port : default_port(); }

    // "/path" or "/path?query" — the Request-Target for HTTP.
    std::string target() const;
};

} // namespace nx::network::http
