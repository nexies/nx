#include <nx/network/tcp/reconnector.hpp>

namespace nx::network::tcp {

reconnector::reconnector(tcp::socket * socket, nx::core::object * parent)
    : nx::core::object(parent)
    , socket_(socket)
{
    nx::core::connect(socket_, &tcp::socket::connected,
                      this, [this]() {
        attempt_ = 0;
        NX_EMIT(connected)
    });

    nx::core::connect(socket_, &tcp::socket::disconnected,
                      this, [this]() {
        if (active_) _schedule_retry();
        else         NX_EMIT(disconnected)
    });

    nx::core::connect(socket_, &tcp::socket::error_occurred,
                      this, [this](nx::error e) {
        if (active_) _schedule_retry();
        else         NX_EMIT(error_occurred, e)
    });

    nx::core::connect(&timer_, &nx::core::timer::timeout,
                      this, [this]() { _try_connect(); });
}

// ── Control ───────────────────────────────────────────────────────────────────

void reconnector::connect(const endpoint & ep)
{
    ep_      = ep;
    active_  = true;
    attempt_ = 0;
    _try_connect();
}

void reconnector::disconnect()
{
    active_ = false;
    timer_.stop();
    socket_->disconnect();
    NX_EMIT(disconnected)
}

// ── Private ───────────────────────────────────────────────────────────────────

void reconnector::_try_connect()
{
    socket_->close();

    if (auto r = socket_->open(); !r) {
        _schedule_retry();
        return;
    }

    socket_->connect(ep_)
        .or_else([this](nx::error) { _schedule_retry(); });
}

void reconnector::_schedule_retry()
{
    const int delay_s = std::min(1 << attempt_, max_delay_s);
    if (attempt_ < 5) ++attempt_; // cap bit-shift at 2^5 = 32 > max_delay_s

    timer_.start(std::chrono::seconds(delay_s));
}

} // namespace nx::network::tcp
