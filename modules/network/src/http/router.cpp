#include <nx/network/http/router.hpp>

#include <algorithm>
#include <sstream>

namespace nx::network::http {

// ── Registration ──────────────────────────────────────────────────────────────

router & router::get (std::string_view p, handler_t h) { return _add("GET",    p, std::move(h)); }
router & router::post(std::string_view p, handler_t h) { return _add("POST",   p, std::move(h)); }
router & router::put (std::string_view p, handler_t h) { return _add("PUT",    p, std::move(h)); }
router & router::del (std::string_view p, handler_t h) { return _add("DELETE", p, std::move(h)); }
router & router::any (std::string_view p, handler_t h) { return _add("",       p, std::move(h)); }

router & router::not_found(handler_t h)
{
    not_found_handler_ = std::move(h);
    return *this;
}

// ── Dispatch ──────────────────────────────────────────────────────────────────

void router::handle(request req, responder_t respond) const
{
    std::unordered_map<std::string, std::string> matched_params;

    for (const auto & route : routes_) {
        matched_params.clear();
        if (_match(route, req.method, req.path(), matched_params)) {
            req.params = std::move(matched_params);
            route.handler(req, std::move(respond));
            return;
        }
    }

    // No match — call custom or default 404 handler.
    if (not_found_handler_)
        not_found_handler_(req, std::move(respond));
    else
        respond(response::not_found());
}

// ── Private helpers ───────────────────────────────────────────────────────────

router & router::_add(std::string_view method, std::string_view pattern, handler_t h)
{
    routes_.push_back(_compile(method, pattern, std::move(h)));
    return *this;
}

router::route router::_compile(std::string_view method,
                                 std::string_view pattern,
                                 handler_t        h)
{
    route r;
    r.method  = std::string(method);
    r.handler = std::move(h);

    // Split pattern on '/' and identify :param and * segments.
    std::string_view rest = pattern;
    if (!rest.empty() && rest.front() == '/')
        rest.remove_prefix(1);

    while (!rest.empty()) {
        const auto slash = rest.find('/');
        const auto seg   = rest.substr(0, slash);

        if (seg == "*") {
            r.wildcard = true;
            break;
        } else if (!seg.empty() && seg.front() == ':') {
            r.param_names.emplace_back(seg.substr(1));
            r.segments.emplace_back(":"); // placeholder
        } else {
            r.param_names.emplace_back();
            r.segments.emplace_back(seg);
        }

        if (slash == std::string_view::npos) break;
        rest = rest.substr(slash + 1);
    }

    return r;
}

bool router::_match(const route & r,
                     std::string_view method,
                     std::string_view path,
                     std::unordered_map<std::string, std::string> & out)
{
    if (!r.method.empty() && r.method != method)
        return false;

    // Split path on '/'.
    std::vector<std::string_view> path_segs;
    std::string_view rest = path;
    if (!rest.empty() && rest.front() == '/')
        rest.remove_prefix(1);

    while (!rest.empty()) {
        const auto slash = rest.find('/');
        path_segs.push_back(rest.substr(0, slash));
        if (slash == std::string_view::npos) break;
        rest = rest.substr(slash + 1);
    }

    if (r.wildcard) {
        if (path_segs.size() <= r.segments.size()) return false;
    } else {
        if (path_segs.size() != r.segments.size()) return false;
    }

    for (std::size_t i = 0; i < r.segments.size(); ++i) {
        if (r.segments[i] == ":") {
            out[r.param_names[i]] = std::string(path_segs[i]);
        } else if (r.segments[i] != path_segs[i]) {
            return false;
        }
    }

    return true;
}

} // namespace nx::network::http
