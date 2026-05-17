#pragma once

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <cstddef>

namespace nx::network::http {

struct request {
    std::string method;   // "GET", "POST", …
    std::string target;   // "/path?query"
    std::string version;  // "1.0" or "1.1"

    std::unordered_map<std::string, std::string> headers; // names lowercased
    std::vector<char> body;

    // Convenience accessors
    std::string_view path()                        const noexcept;
    std::string_view query()                       const noexcept;
    std::string_view header(std::string_view name) const noexcept; // case-insensitive
    std::size_t      content_length()              const noexcept;
    bool             keep_alive()                  const noexcept;
};

} // namespace nx::network::http
