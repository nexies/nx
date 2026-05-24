#include <nx/tg/bot.hpp>

#include <nlohmann/json.hpp>

#include <stdexcept>
#include <string>

namespace nx::tg {

namespace {

using json = nlohmann::json;

user parse_user(const json & j)
{
    user u;
    u.id         = j.at("id").get<int64_t>();
    u.is_bot     = j.value("is_bot", false);
    u.first_name = j.value("first_name", std::string{});
    u.last_name  = j.value("last_name",  std::string{});
    u.username   = j.value("username",   std::string{});
    return u;
}

chat parse_chat(const json & j)
{
    chat c;
    c.id         = j.at("id").get<int64_t>();
    c.type       = j.value("type",       std::string{});
    c.title      = j.value("title",      std::string{});
    c.username   = j.value("username",   std::string{});
    c.first_name = j.value("first_name", std::string{});
    c.last_name  = j.value("last_name",  std::string{});
    return c;
}

message parse_message(const json & j)
{
    message m;
    m.message_id = j.at("message_id").get<int64_t>();
    if (j.contains("from")) m.from = parse_user(j.at("from"));
    m.chat = parse_chat(j.at("chat"));
    m.date = j.value("date", int64_t{ 0 });
    m.text = j.value("text", std::string{});
    return m;
}

} // namespace

// ── Constructor / Destructor ──────────────────────────────────────────────────

bot::bot(std::string token, nx::core::object * parent)
    : nx::core::object(parent)
    , token_(std::move(token))
    , tls_ctx_(nx::network::tls::context::role::client)
    , poll_client_([this](const std::string & h) { return _make_stream(h); }, this)
    , send_client_([this](const std::string & h) { return _make_stream(h); }, this)
{}

bot::~bot()
{
    stop();
}

// ── Control ───────────────────────────────────────────────────────────────────

void bot::start()
{
    if (running_) return;
    running_ = true;
    _poll();
}

void bot::stop()
{
    running_ = false;
}

// ── send_message ──────────────────────────────────────────────────────────────

void bot::send_message(int64_t chat_id, const std::string & text, send_callback cb)
{
    send_queue_.push_back({ chat_id, text, std::move(cb) });
    _flush_send_queue();
}

// ── Private helpers ───────────────────────────────────────────────────────────

std::shared_ptr<bot::tls_stream> bot::_make_stream(const std::string & host)
{
    auto s = std::make_shared<tls_stream>(tls_ctx_);
    s->set_hostname(host);
    return s;
}

std::string bot::_api_url(std::string_view method) const
{
    return "https://api.telegram.org/bot" + token_ + "/" + std::string(method);
}

// ── Long polling ──────────────────────────────────────────────────────────────

void bot::_poll()
{
    const std::string url_str =
        _api_url("getUpdates?timeout=30&offset=" + std::to_string(offset_));

    auto url_r = nx::network::http::url::parse(url_str);
    if (!url_r) {
        NX_EMIT(error_occurred, url_r.error())
        return;
    }

    poll_client_.get(url_r.value(),
        [this](nx::network::http::response resp, nx::error err) {
            _on_poll_response(std::move(resp), err);
        });
}

void bot::_on_poll_response(nx::network::http::response resp, nx::error err)
{
    if (err) {
        NX_EMIT(error_occurred, err)
        if (running_) _poll();
        return;
    }

    try {
        const auto j = json::parse(resp.body);

        if (!j.value("ok", false)) {
            const std::string desc = j.value("description", std::string("unknown error"));
            NX_EMIT(error_occurred, nx::err::runtime_error("getUpdates: " + desc))
        } else {
            for (const auto & upd : j.at("result")) {
                const int64_t uid = upd.at("update_id").get<int64_t>();
                if (uid >= offset_)
                    offset_ = uid + 1;

                if (upd.contains("message"))
                    NX_EMIT(message_received, parse_message(upd.at("message")))
            }
        }
    } catch (const std::exception & e) {
        NX_EMIT(error_occurred, nx::err::runtime_error(
            std::string("getUpdates parse error: ") + e.what()))
    }

    if (running_) _poll();
}

// ── Send queue ────────────────────────────────────────────────────────────────

void bot::_flush_send_queue()
{
    if (send_in_progress_ || send_queue_.empty()) return;

    send_in_progress_ = true;

    auto item = std::move(send_queue_.front());
    send_queue_.pop_front();

    json body;
    body["chat_id"] = item.chat_id;
    body["text"]    = item.text;

    auto url_r = nx::network::http::url::parse(_api_url("sendMessage"));
    if (!url_r) {
        send_in_progress_ = false;
        if (item.cb) item.cb({}, url_r.error());
        _flush_send_queue();
        return;
    }

    send_client_.post(url_r.value(), body.dump(), "application/json",
        [this, cb = std::move(item.cb)](nx::network::http::response resp, nx::error err) {
            _on_send_response(std::move(resp), err, std::move(cb));
        });
}

void bot::_on_send_response(nx::network::http::response resp, nx::error err,
                             send_callback cb)
{
    send_in_progress_ = false;

    if (cb) {
        if (err) {
            cb({}, err);
        } else {
            try {
                const auto j = json::parse(resp.body);
                if (!j.value("ok", false)) {
                    const std::string desc = j.value("description", std::string("unknown error"));
                    cb({}, nx::err::runtime_error("sendMessage: " + desc));
                } else {
                    cb(parse_message(j.at("result")), {});
                }
            } catch (const std::exception & e) {
                cb({}, nx::err::runtime_error(
                    std::string("sendMessage parse error: ") + e.what()));
            }
        }
    }

    _flush_send_queue();
}

} // namespace nx::tg
