#pragma once

#include <nx/network/pcap/packet.hpp>
#include <nx/common/types/result.hpp>

#include <nx/core2/object/object.hpp>

#include <atomic>
#include <cstdint>
#include <memory>

namespace nx::network::pcap {

class file_reader;
class device;

// ── replay_stats ──────────────────────────────────────────────────────────────

struct replay_stats {
    uint64_t packets_sent    = 0;
    uint64_t packets_skipped = 0; // oversized or inject error
    uint64_t bytes_sent      = 0;
    uint64_t elapsed_us      = 0; // wall-clock microseconds since start
    float    pps             = 0; // packets per second (1-sec rolling)
    float    mbps            = 0; // megabits per second (1-sec rolling)
    float    progress        = 0; // 0.0 – 1.0 (based on packet count if known)
};

// ── replayer ──────────────────────────────────────────────────────────────────
//
// Replays a pcap capture file through a network interface at a controlled rate.
//
// Threading model:
//   - start() launches two threads: a reader thread and a sender thread.
//   - Threads communicate via a lock-free SPSC ring of packet_batch pointers.
//   - The caller's thread (asio event loop) receives stats_updated signals
//     via a periodic timer every stats_interval_ms milliseconds.
//   - finished() and error() are also emitted on the caller's thread.
//
// Rate control:
//   set_rate(1.0f)  — original capture speed (inter-packet timestamps honoured)
//   set_rate(2.0f)  — 2× faster (gaps halved)
//   set_rate(0.0f)  — maximum speed (no inter-packet delay)
//
// Usage:
//   replayer rp(reader, dev, nx::core::thread::current_context());
//   rp.set_rate(1.0f);
//   nx::core::connect(&rp, &replayer::stats_updated, &view, &ui_view::on_stats);
//   nx::core::connect(&rp, &replayer::finished,      &app,  &App::quit);
//   rp.start();

class replayer : public nx::core::object {
public:
    NX_OBJECT(replayer)
    NX_DISABLE_COPY(replayer)
    NX_DISABLE_MOVE(replayer)

    // reader and dev must outlive the replayer.
    replayer(file_reader &        reader,
             device &             dev,
             nx::core::object *   parent = nullptr);

    ~replayer() override;

    // ── Configuration (call before start) ────────────────────────────────────

    // Playback speed multiplier.  0.0 = maximum rate (no timing).
    void set_rate(float rate) noexcept;

    // How often stats_updated is emitted (default: 200 ms).
    void set_stats_interval(int ms) noexcept;

    // Ring buffer depth in batches (default: 8, must be power of two).
    void set_ring_depth(uint32_t depth) noexcept;

    // ── Lifecycle ─────────────────────────────────────────────────────────────

    nx::result<void> start();
    void             stop();
    void             pause();
    void             resume();

    [[nodiscard]] bool is_running() const noexcept;
    [[nodiscard]] bool is_paused()  const noexcept;

    // ── Signals (emitted on the caller's asio thread) ─────────────────────────

    NX_SIGNAL(stats_updated, replay_stats)
    NX_SIGNAL(finished)
    NX_SIGNAL(error, nx::error)

private:
    void _arm_stats_timer();

    struct impl;
    std::unique_ptr<impl> impl_;
};

} // namespace nx::network::pcap
