#include <nx/network/http/url.hpp>

#include <stdexcept>

namespace nx::network::http {

nx::result<url> url::parse(std::string_view raw)
{
    url result;

    // scheme://
    const auto scheme_end = raw.find("://");
    if (scheme_end == std::string_view::npos)
        return nx::err::runtime_error("url: missing '://'");

    result.scheme = std::string(raw.substr(0, scheme_end));
    raw.remove_prefix(scheme_end + 3);

    // host[:port]  (up to first '/' or end-of-string)
    const auto path_start = raw.find('/');
    const auto authority  = path_start == std::string_view::npos
                                ? raw
                                : raw.substr(0, path_start);

    const auto colon = authority.rfind(':');
    if (colon != std::string_view::npos) {
        result.host = std::string(authority.substr(0, colon));
        try {
            const int p = std::stoi(std::string(authority.substr(colon + 1)));
            if (p <= 0 || p > 65535)
                return nx::err::runtime_error("url: port out of range");
            result.port = static_cast<uint16_t>(p);
        } catch (...) {
            return nx::err::runtime_error("url: invalid port");
        }
    } else {
        result.host = std::string(authority);
    }

    if (result.host.empty())
        return nx::err::runtime_error("url: empty host");

    if (path_start == std::string_view::npos) {
        result.path = "/";
        return result;
    }

    raw.remove_prefix(path_start);

    // path[?query]
    const auto query_start = raw.find('?');
    if (query_start == std::string_view::npos) {
        result.path = std::string(raw);
    } else {
        result.path  = std::string(raw.substr(0, query_start));
        result.query = std::string(raw.substr(query_start + 1));
    }

    if (result.path.empty())
        result.path = "/";

    return result;
}

std::string url::target() const
{
    if (query.empty()) return path;
    return path + '?' + query;
}

} // namespace nx::network::http
