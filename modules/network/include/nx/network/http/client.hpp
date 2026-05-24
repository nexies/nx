#pragma once

#include <nx/network/http/url.hpp>
#include <nx/network/http/request.hpp>
#include <nx/network/http/response.hpp>
#include <nx/network/http/response_parser.hpp>
#include <nx/network/buffer/read_accumulator.hpp>
#include <nx/network/resolver.hpp>
#include <nx/network/tcp/socket.hpp>

#include <nx/common/helpers.hpp>
#include <nx/common/types/result.hpp>
#include <nx/core2/object/object.hpp>

#include <functional>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

namespace nx::network::http {

// ── http::client<Stream> ──────────────────────────────────────────────────────
//
// Async HTTP/1.1 client.  One outstanding request at a time.
//
// Plain HTTP:
//   http::client<> client;
//   client.get(http::url::parse("http://example.com/").value(), cb);
//
// HTTPS:
//   tls::context ctx;
//   http::client<tls::stream<tcp::socket>> client(
//       [&ctx](const std::string & host) {
//           auto s = std::make_shared<tls::stream<tcp::socket>>(ctx);
//           s->set_hostname(host);
//           return s;
//       });

template<typename Stream = tcp::socket>
class client : public nx::core::object {
public:
    NX_OBJECT(client)
    NX_DISABLE_COPY(client)
    NX_DISABLE_MOVE(client)

    using callback_t     = std::function<void(http::response, nx::error)>;
    using stream_factory = std::function<std::shared_ptr<Stream>(const std::string & host)>;

    // ── Plain TCP constructor (only when Stream == tcp::socket) ───────────────

    template<typename S = Stream,
             typename   = std::enable_if_t<std::is_same_v<S, tcp::socket>>>
    explicit client(nx::core::object * parent = nullptr)
        : client([](const std::string &){ return std::make_shared<Stream>(); }, parent)
    {}

    // ── General constructor ───────────────────────────────────────────────────

    explicit client(stream_factory factory, nx::core::object * parent = nullptr)
        : nx::core::object(parent)
        , factory_(std::move(factory))
        , resolver_(this)
    {
        nx::core::connect(&resolver_, &resolver::resolved, this,
            [this](std::string host, std::vector<endpoint> eps) {
                _on_resolved(std::move(host), std::move(eps));
            });

        nx::core::connect(&resolver_, &resolver::error_occurred, this,
            [this](nx::error e) { _fail(e); });
    }

    // ── Request API ───────────────────────────────────────────────────────────

    void get(const url & u, callback_t cb)
    {
        http::request req;
        req.method  = "GET";
        req.target  = u.target();
        req.version = "1.1";
        req.headers["host"]       = u.host;
        req.headers["connection"] = "close";
        _start(u, std::move(req), std::move(cb));
    }

    void post(const url & u, std::string body,
              std::string_view content_type, callback_t cb)
    {
        http::request req;
        req.method  = "POST";
        req.target  = u.target();
        req.version = "1.1";
        req.headers["host"]           = u.host;
        req.headers["connection"]     = "close";
        req.headers["content-type"]   = std::string(content_type);
        req.headers["content-length"] = std::to_string(body.size());
        req.body.assign(body.begin(), body.end());
        _start(u, std::move(req), std::move(cb));
    }

private:
    // ── Internal state ────────────────────────────────────────────────────────

    void _start(const url & u, http::request req, callback_t cb)
    {
        pending_req_ = std::move(req);
        pending_cb_  = std::move(cb);
        resolver_.resolve(u.host, u.effective_port());
    }

    void _on_resolved(std::string host, std::vector<endpoint> eps)
    {
        if (eps.empty()) { _fail(nx::err::runtime_error("resolve: no endpoints")); return; }

        stream_ = factory_(host);
        if (auto * t = get_thread())
            stream_->move_to_thread(t);

        acc_.reset(new read_accumulator());
        parser_.reset(new response_parser(acc_.get()));  // wires data_available internally

        nx::core::connect(stream_.get(), &Stream::data_received, acc_.get(),
            [a = acc_.get()](nx::span<const char> data) { a->feed(data); });

        nx::core::connect(stream_.get(), &Stream::connected, this,
            [this]() { _on_connected(); });

        nx::core::connect(stream_.get(), &Stream::disconnected, this,
            [this]() { _on_disconnected(); });

        nx::core::connect(stream_.get(), &Stream::error_occurred, this,
            [this](nx::error e) { _fail(e); });

        nx::core::connect(parser_.get(), &response_parser::response_ready, this,
            [this](http::response r) { _on_response(std::move(r)); });

        nx::core::connect(parser_.get(), &response_parser::parse_error, this,
            [this](nx::error e) { _fail(e); });

        stream_->open();
        stream_->connect(eps.front());
    }

    void _on_connected()
    {
        // Serialize and send the pending request.
        std::string wire;
        wire.reserve(256 + pending_req_.body.size());

        wire += pending_req_.method + ' ' + pending_req_.target + " HTTP/1.1\r\n";
        for (const auto & [k, v] : pending_req_.headers)
            wire += k + ": " + v + "\r\n";
        wire += "\r\n";
        if (!pending_req_.body.empty())
            wire.append(pending_req_.body.begin(), pending_req_.body.end());

        stream_->write(wire.data(), wire.size());
    }

    void _on_response(http::response r)
    {
        auto cb = std::move(pending_cb_);
        _cleanup();
        if (cb) cb(std::move(r), {});
    }

    void _on_disconnected()
    {
        if (!pending_cb_) return;
        // Server closed the connection — ask the parser to finalise with
        // whatever body it has (handles connection-close responses).
        if (parser_) {
            parser_->finalize();
            return; // if the response was complete, _on_response fired above
        }
        _fail(nx::err::runtime_error("http::client: disconnected before response"));
    }

    void _fail(nx::error e)
    {
        auto cb = std::move(pending_cb_);
        _cleanup();
        if (cb) cb({}, e);
    }

    // Moves owned resources out of the client and schedules their teardown on
    // the next event-loop tick.  This prevents destroying objects (acc_, parser_,
    // stream_) while they are still on the call stack (signal dispatch).
    void _cleanup()
    {
        pending_cb_  = nullptr;
        pending_req_ = {};

        auto p = std::shared_ptr<response_parser>(parser_.release());
        auto a = std::shared_ptr<read_accumulator>(acc_.release());
        auto s = std::move(stream_);

        if (auto * t = get_thread()) {
            t->post([p, a, s]() mutable { if (s) s->disconnect(); });
        } else {
            if (s) s->disconnect();
        }
    }

    // ── Data members ──────────────────────────────────────────────────────────

    stream_factory                    factory_;
    resolver                          resolver_;

    std::shared_ptr<Stream>           stream_;
    std::unique_ptr<read_accumulator> acc_;
    std::unique_ptr<response_parser>  parser_;

    http::request                     pending_req_;
    callback_t                        pending_cb_;
};

} // namespace nx::network::http
