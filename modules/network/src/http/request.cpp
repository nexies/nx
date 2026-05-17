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

std::string_view request::param(std::string_view name) const noexcept
{
    const auto it = params.find(std::string(name));
    if (it == params.end()) return {};
    return it->second;
}

std::string_view request::body_sv() const noexcept
{
    return { body.data(), body.size() };
}

// ── query_params ──────────────────────────────────────────────────────────────

static std::string url_decode(std::string_view s)
{
    std::string out;
    out.reserve(s.size());
    for (std::size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '+') {
            out += ' ';
        } else if (s[i] == '%' && i + 2 < s.size()) {
            const char hex[3] = { s[i+1], s[i+2], '\0' };
            out += static_cast<char>(std::strtol(hex, nullptr, 16));
            i += 2;
        } else {
            out += s[i];
        }
    }
    return out;
}

std::unordered_map<std::string, std::string> request::query_params() const
{
    std::unordered_map<std::string, std::string> result;
    const auto q = query();
    if (q.empty()) return result;

    std::size_t pos = 0;
    while (pos < q.size()) {
        const auto amp = q.find('&', pos);
        const auto end = (amp == std::string_view::npos) ? q.size() : amp;
        const auto pair = q.substr(pos, end - pos);
        const auto eq   = pair.find('=');
        if (eq != std::string_view::npos)
            result[url_decode(pair.substr(0, eq))] = url_decode(pair.substr(eq + 1));
        else if (!pair.empty())
            result[url_decode(pair)] = {};
        pos = (amp == std::string_view::npos) ? q.size() : amp + 1;
    }
    return result;
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
