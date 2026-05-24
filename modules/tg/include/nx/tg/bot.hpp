#pragma once

#include <nx/tg/types.hpp>

#include <nx/network/http/client.hpp>
#include <nx/network/http/response.hpp>
#include <nx/network/tls/tls.hpp>
#include <nx/network/tcp/socket.hpp>

#include <nx/common/helpers.hpp>
#include <nx/common/types/result.hpp>
#include <nx/core2/object/object.hpp>

#include <deque>
#include <functional>
#include <memory>
#include <string>

namespace nx::tg {

// ── tg::bot ───────────────────────────────────────────────────────────────────
//
// Async Telegram Bot API client.  Uses long polling (getUpdates) to receive
// updates and a serialised send queue for outgoing messages.
//
// Usage:
//   tg::bot bot(token);
//   nx::core::connect(&bot, &tg::bot::message_received, &bot,
//       [&bot](const tg::message & msg) {
//           bot.send_message(msg.chat.id, "Hello!");
//       });
//   bot.start();
//   return loop.exec();

class bot : public nx::core::object {
public:
    NX_OBJECT(bot)
    NX_DISABLE_COPY(bot)
    NX_DISABLE_MOVE(bot)

    using send_callback = std::function<void(message, nx::error)>;

    using tls_stream   = nx::network::tls::stream<nx::network::tcp::socket>;
    using https_client = nx::network::http::client<tls_stream>;

    explicit bot(std::string token, nx::core::object * parent = nullptr);
    ~bot() override;

    // ── Control ───────────────────────────────────────────────────────────────

    void start();
    void stop();

    // ── Outgoing messages ─────────────────────────────────────────────────────

    void send_message(int64_t chat_id, const std::string & text,
                      send_callback cb = {});

    // ── Signals ───────────────────────────────────────────────────────────────

    NX_SIGNAL(message_received, tg::message)
    NX_SIGNAL(error_occurred,   nx::error)

private:
    std::shared_ptr<tls_stream> _make_stream(const std::string & host);
    std::string                 _api_url(std::string_view method) const;

    void _poll();
    void _on_poll_response(nx::network::http::response resp, nx::error err);

    void _flush_send_queue();
    void _on_send_response(nx::network::http::response resp, nx::error err,
                           send_callback cb);

    // ── Data members (init order matters) ─────────────────────────────────────

    std::string token_;
    int64_t     offset_          { 0 };
    bool        running_         { false };
    bool        send_in_progress_{ false };

    nx::network::tls::context tls_ctx_;
    https_client              poll_client_;
    https_client              send_client_;

    struct pending_send {
        int64_t       chat_id;
        std::string   text;
        send_callback cb;
    };
    std::deque<pending_send> send_queue_;
};

} // namespace nx::tg
