#include <nx/network/http/static_handler.hpp>

#include <fstream>
#include <iterator>
#include <unordered_map>

namespace nx::network::http {

static_handler::static_handler(std::filesystem::path root)
    : root_(std::move(root))
{}

void static_handler::handle(const request & req, responder_t respond) const
{
    // Strip query string and leading slash to get a relative path component.
    std::string_view rel = req.path();
    if (!rel.empty() && rel.front() == '/')
        rel.remove_prefix(1);
    if (rel.empty())
        rel = "index.html";

    const std::filesystem::path target = root_ / std::string(rel);

    // Reject traversal attempts.
    const auto canonical_root   = std::filesystem::weakly_canonical(root_);
    const auto canonical_target = std::filesystem::weakly_canonical(target);
    const auto [root_end, _]    = std::mismatch(canonical_root.begin(),  canonical_root.end(),
                                                 canonical_target.begin(), canonical_target.end());
    if (root_end != canonical_root.end()) {
        respond(response::forbidden());
        return;
    }

    std::ifstream file(target, std::ios::binary);
    if (!file) {
        respond(response::not_found());
        return;
    }

    std::string body(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>{});

    const auto ext = target.extension().string(); // e.g. ".html"
    const auto mime = mime_type(ext.empty() ? ext : std::string_view(ext).substr(1));

    response res;
    res.status_code = 200;
    res.reason      = "OK";
    res.headers["Content-Type"]   = std::string(mime);
    res.headers["Content-Length"] = std::to_string(body.size());
    res.body = std::move(body);
    respond(std::move(res));
}

std::string_view static_handler::mime_type(std::string_view ext) noexcept
{
    static const std::unordered_map<std::string_view, std::string_view> table {
        { "html",  "text/html; charset=utf-8"       },
        { "htm",   "text/html; charset=utf-8"       },
        { "css",   "text/css; charset=utf-8"        },
        { "js",    "text/javascript; charset=utf-8" },
        { "mjs",   "text/javascript; charset=utf-8" },
        { "json",  "application/json"               },
        { "txt",   "text/plain; charset=utf-8"      },
        { "xml",   "application/xml"                },
        { "svg",   "image/svg+xml"                  },
        { "png",   "image/png"                      },
        { "jpg",   "image/jpeg"                     },
        { "jpeg",  "image/jpeg"                     },
        { "gif",   "image/gif"                      },
        { "webp",  "image/webp"                     },
        { "ico",   "image/x-icon"                   },
        { "woff",  "font/woff"                      },
        { "woff2", "font/woff2"                      },
        { "ttf",   "font/ttf"                       },
        { "otf",   "font/otf"                       },
        { "pdf",   "application/pdf"                },
        { "zip",   "application/zip"                },
        { "gz",    "application/gzip"               },
    };

    const auto it = table.find(ext);
    return it != table.end() ? it->second : "application/octet-stream";
}

} // namespace nx::network::http
