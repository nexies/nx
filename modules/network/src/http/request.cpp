#include <nx/network/http/request.hpp>

#include <algorithm>
#include <stdexcept>

namespace nx::network::http {

std::string_view request::path() const noexcept
{
    const auto q = target.find('?');
    if (q == std::string::npos) return target;
    return std::string_view(target).substr(0, q);
}

std::string_view request::query() const noexcept
{
    const auto q = target.find('?');
    if (q == std::string::npos) return {};
    return std::string_view(target).substr(q + 1);
}

std::string_view request::header(std::string_view name) const noexcept
{
    std::string key(name.size(), '\0');
    std::transform(name.begin(), name.end(), key.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    const auto it = headers.find(key);
    if (it == headers.end()) return {};
    return it->second;
}

std::size_t request::content_length() const noexcept
{
    const auto it = headers.find("content-length");
    if (it == headers.end()) return 0;
    try { return static_cast<std::size_t>(std::stoull(it->second)); }
    catch (...) { return 0; }
}

bool request::keep_alive() const noexcept
{
    const auto it = headers.find("connection");
    if (version == "1.1")
        return it == headers.end() || it->second != "close";
    else
        return it != headers.end() && it->second == "keep-alive";
}

} // namespace nx::network::http
