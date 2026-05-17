#include <nx/network/http/response.hpp>

#include <algorithm>

namespace nx::network::http {

// ── Factories ─────────────────────────────────────────────────────────────────

response response::ok(std::string body, std::string_view content_type)
{
    response r;
    r.status_code = 200;
    r.reason      = "OK";
    r.body        = std::move(body);
    if (!r.body.empty())
        r.headers["Content-Type"] = std::string(content_type);
    return r;
}

response response::not_found(std::string body)
{
    response r;
    r.status_code = 404;
    r.reason      = "Not Found";
    r.body        = std::move(body);
    if (!r.body.empty())
        r.headers["Content-Type"] = "text/plain";
    return r;
}

response response::forbidden(std::string body)
{
    response r;
    r.status_code = 403;
    r.reason      = "Forbidden";
    r.body        = std::move(body);
    if (!r.body.empty())
        r.headers["Content-Type"] = "text/plain";
    return r;
}

response response::bad_request(std::string body)
{
    response r;
    r.status_code = 400;
    r.reason      = "Bad Request";
    r.body        = std::move(body);
    if (!r.body.empty())
        r.headers["Content-Type"] = "text/plain";
    return r;
}

response response::internal_error(std::string body)
{
    response r;
    r.status_code = 500;
    r.reason      = "Internal Server Error";
    r.body        = std::move(body);
    if (!r.body.empty())
        r.headers["Content-Type"] = "text/plain";
    return r;
}

// ── Serialization ─────────────────────────────────────────────────────────────

std::string response::serialize() const
{
    std::string out;
    out.reserve(256 + body.size());

    out += "HTTP/1.1 ";
    out += std::to_string(status_code);
    out += ' ';
    out += reason;
    out += "\r\n";

    bool has_content_length = false;
    for (const auto & [k, v] : headers) {
        std::string kl(k.size(), '\0');
        std::transform(k.begin(), k.end(), kl.begin(),
                       [](unsigned char c){ return std::tolower(c); });
        if (kl == "content-length") has_content_length = true;

        out += k; out += ": "; out += v; out += "\r\n";
    }

    if (!has_content_length) {
        out += "Content-Length: ";
        out += std::to_string(body.size());
        out += "\r\n";
    }

    out += "\r\n";
    out += body;
    return out;
}

} // namespace nx::network::http
