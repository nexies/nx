#pragma once

#include <nx/network/tls/context.hpp>
#include <nx/network/types.hpp>

#include <nx/common/helpers.hpp>
#include <nx/common/span.hpp>
#include <nx/common/types/result.hpp>

#include <nx/core2/object/object.hpp>

#include <openssl/ssl.h>
#include <openssl/err.h>

#include <memory>
#include <string>

namespace nx::network::tls {

// ── tls::stream<Socket> ───────────────────────────────────────────────────────
//
// Wraps any socket type that exposes the tcp::socket signal interface and
// adds TLS on top via OpenSSL memory BIOs.
//
// Client mode (default): creates its own Socket; call open()/connect() as
// you would with tcp::socket.  The `connected` signal fires after the TLS
// handshake completes (not after TCP connect).
//
// Server mode: accepts an already-accepted shared_ptr<Socket>; the TLS
// handshake begins immediately on construction.  `connected` fires when done.
//
// In both modes write() / disconnect() and signals (data_received,
// disconnected, error_occurred) mirror the tcp::socket interface.

template<typename Socket>
class stream : public nx::core::object {
public:
    NX_OBJECT(stream)
    NX_DISABLE_COPY(stream)
    NX_DISABLE_MOVE(stream)

    // ── Client mode ───────────────────────────────────────────────────────────

    explicit stream(context & ctx, nx::core::object * parent = nullptr)
        : nx::core::object(parent)
        , ctx_(ctx)
        , socket_(std::make_shared<Socket>())
    {
        _wire_socket();
    }

    // ── Server mode ───────────────────────────────────────────────────────────

    stream(std::shared_ptr<Socket> sock, context & ctx,
           nx::core::object * parent = nullptr)
        : nx::core::object(parent)
        , ctx_(ctx)
        , socket_(std::move(sock))
    {
        _wire_socket();
        _start_ssl(false);  // server mode
        _pump_handshake();
    }

    ~stream() override
    {
        if (ssl_) {
            SSL_free(ssl_);
            ssl_ = nullptr;
        }
    }

    // ── Client-mode socket operations ─────────────────────────────────────────

    // Set SNI hostname before calling connect().
    void set_hostname(const std::string & host) { hostname_ = host; }

    nx::result<void> open()
    {
        return socket_->open();
    }

    nx::result<void> connect(const endpoint & ep)
    {
        return socket_->connect(ep);
    }

    void disconnect()
    {
        if (ssl_ && hs_ == hs_state::done)
            SSL_shutdown(ssl_);
        socket_->disconnect();
    }

    // Encrypt and send data.  Only valid after `connected` fires.
    nx::result<std::size_t> write(const char * data, std::size_t len)
    {
        if (!ssl_ || hs_ != hs_state::done)
            return nx::err::runtime_error("tls::stream: not connected");
        const int n = SSL_write(ssl_, data, static_cast<int>(len));
        if (n < 0) return _make_ssl_error("SSL_write");
        _flush_write_bio();
        return static_cast<std::size_t>(n);
    }

    // ── Signals (same names as tcp::socket) ───────────────────────────────────

    NX_SIGNAL(connected)
    NX_SIGNAL(disconnected)
    NX_SIGNAL(data_received, nx::span<const char>)
    NX_SIGNAL(error_occurred, nx::error)

    // ── Access to underlying socket ───────────────────────────────────────────

    Socket       & socket()       noexcept { return *socket_; }
    const Socket & socket() const noexcept { return *socket_; }

private:
    enum class hs_state { pending, done, failed };

    // ── Setup ─────────────────────────────────────────────────────────────────

    void _wire_socket()
    {
        nx::core::connect(socket_.get(), &Socket::connected, this,
            [this]() { _on_socket_connected(); });

        nx::core::connect(socket_.get(), &Socket::data_received, this,
            [this](nx::span<const char> data) { _on_socket_data(data); });

        nx::core::connect(socket_.get(), &Socket::disconnected, this,
            [this]() { NX_EMIT(disconnected) });

        nx::core::connect(socket_.get(), &Socket::error_occurred, this,
            [this](nx::error e) { NX_EMIT(error_occurred, e) });
    }

    void _start_ssl(bool is_client)
    {
        ssl_ = SSL_new(ctx_.native_handle());
        if (!ssl_) {
            NX_EMIT(error_occurred, nx::err::runtime_error("SSL_new failed"))
            return;
        }

        // Memory BIOs — SSL_set_bio takes ownership of both.
        BIO * rbio = BIO_new(BIO_s_mem());
        BIO * wbio = BIO_new(BIO_s_mem());
        SSL_set_bio(ssl_, rbio, wbio);

        // Keep pointers for feeding data in / draining data out.
        read_bio_  = rbio;
        write_bio_ = wbio;

        if (is_client) {
            SSL_set_connect_state(ssl_);
            if (!hostname_.empty())
                SSL_set_tlsext_host_name(ssl_, hostname_.c_str());
        } else {
            SSL_set_accept_state(ssl_);
        }
    }

    // ── Socket event handlers ─────────────────────────────────────────────────

    void _on_socket_connected()
    {
        _start_ssl(true);  // client mode
        _pump_handshake();
    }

    void _on_socket_data(nx::span<const char> data)
    {
        if (!ssl_) return;
        BIO_write(read_bio_, data.data(), static_cast<int>(data.size()));

        if (hs_ == hs_state::pending)
            _pump_handshake();
        else if (hs_ == hs_state::done)
            _pump_read();
    }

    // ── TLS state machine ─────────────────────────────────────────────────────

    void _pump_handshake()
    {
        const int r = SSL_do_handshake(ssl_);
        _flush_write_bio();

        if (r == 1) {
            hs_ = hs_state::done;
            NX_EMIT(connected)
            _pump_read();
            return;
        }

        const int err = SSL_get_error(ssl_, r);
        if (err != SSL_ERROR_WANT_READ && err != SSL_ERROR_WANT_WRITE) {
            hs_ = hs_state::failed;
            NX_EMIT(error_occurred, _make_ssl_error("handshake"))
        }
        // SSL_ERROR_WANT_READ / WANT_WRITE: normal, wait for more socket data.
    }

    void _pump_read()
    {
        char buf[16384];
        for (;;) {
            const int n = SSL_read(ssl_, buf, static_cast<int>(sizeof(buf)));
            if (n > 0) {
                NX_EMIT(data_received, nx::span<const char>(buf, static_cast<std::size_t>(n)))
                continue;
            }
            const int err = SSL_get_error(ssl_, n);
            if (err == SSL_ERROR_ZERO_RETURN) {
                NX_EMIT(disconnected)
            } else if (err != SSL_ERROR_WANT_READ && err != SSL_ERROR_WANT_WRITE) {
                NX_EMIT(error_occurred, _make_ssl_error("SSL_read"))
            }
            break;
        }
        _flush_write_bio();
    }

    void _flush_write_bio()
    {
        if (!write_bio_) return;
        char buf[16384];
        for (;;) {
            const int n = BIO_read(write_bio_, buf, static_cast<int>(sizeof(buf)));
            if (n <= 0) break;
            socket_->write(buf, static_cast<std::size_t>(n));
        }
    }

    nx::error _make_ssl_error(std::string_view ctx_msg)
    {
        char buf[256] = {};
        ERR_error_string_n(ERR_get_error(), buf, sizeof(buf));
        const std::string msg = ctx_msg.empty()
            ? std::string(buf)
            : std::string(ctx_msg) + ": " + buf;
        return nx::err::runtime_error(msg);
    }

    // ── Data members ──────────────────────────────────────────────────────────

    context &               ctx_;
    std::shared_ptr<Socket> socket_;
    std::string             hostname_;

    SSL *      ssl_       { nullptr };
    BIO *      read_bio_  { nullptr };
    BIO *      write_bio_ { nullptr };
    hs_state   hs_        { hs_state::pending };
};

} // namespace nx::network::tls
