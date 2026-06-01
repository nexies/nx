#include <nx/network/pcap/replayer.hpp>
#include <nx/network/pcap/file_reader.hpp>
#include <nx/network/pcap/device.hpp>

#include <nx/asio/steady_timer.hpp>
#include <nx/core2/thread/thread.hpp>

#include <atomic>
#include <cassert>
#include <chrono>
#include <memory>
#include <thread>
#include <vector>

#if defined(__x86_64__) || defined(__i386__) || defined(_M_X64) || defined(_M_IX86)
#  if defined(_MSC_VER)
#    include <intrin.h>
#  else
#    include <immintrin.h>
#  endif
#  define NX_CPU_PAUSE() _mm_pause()
#else
#  define NX_CPU_PAUSE() std::this_thread::yield()
#endif

namespace nx::network::pcap {

using steady_clock = std::chrono::steady_clock;
using time_point   = steady_clock::time_point;
using micros       = std::chrono::microseconds;

// ── SPSC ring buffer ──────────────────────────────────────────────────────────

class spsc_ring {
public:
    explicit spsc_ring(uint32_t depth)
        : mask_(depth - 1), slots_(depth, nullptr)
    {
        assert((depth & (depth - 1)) == 0 && "depth must be power of two");
    }

    bool push(packet_batch * b) noexcept
    {
        const uint32_t h    = head_.load(std::memory_order_relaxed);
        const uint32_t next = h + 1;
        if (next - tail_.load(std::memory_order_acquire) > mask_ + 1)
            return false;
        slots_[h & mask_] = b;
        head_.store(next, std::memory_order_release);
        return true;
    }

    packet_batch * pop() noexcept
    {
        const uint32_t t = tail_.load(std::memory_order_relaxed);
        if (head_.load(std::memory_order_acquire) == t)
            return nullptr;
        packet_batch * b = slots_[t & mask_];
        tail_.store(t + 1, std::memory_order_release);
        return b;
    }

private:
    const uint32_t              mask_;
    std::vector<packet_batch *> slots_;
    alignas(64) std::atomic<uint32_t> head_ { 0 };
    alignas(64) std::atomic<uint32_t> tail_ { 0 };
};

// ── Batch pool ────────────────────────────────────────────────────────────────

class batch_pool {
public:
    explicit batch_pool(uint32_t count) : free_(count)
    {
        batches_.resize(count);
        for (auto & b : batches_)
            while (!free_.push(&b)) {}
    }

    packet_batch * acquire() noexcept { return free_.pop(); }
    void           release(packet_batch * b) noexcept
    {
        while (!free_.push(b)) NX_CPU_PAUSE();
    }

private:
    spsc_ring                 free_;
    std::vector<packet_batch> batches_;
};

// ── Timing helpers ────────────────────────────────────────────────────────────

static int64_t ts_to_us(const timeval & tv) noexcept
{
    return static_cast<int64_t>(tv.tv_sec) * 1'000'000 + tv.tv_usec;
}

static void sleep_until_tp(time_point target) noexcept
{
    const auto spin_from = target - std::chrono::microseconds(50);
    if (steady_clock::now() < spin_from)
        std::this_thread::sleep_until(spin_from);
    while (steady_clock::now() < target)
        NX_CPU_PAUSE();
}

// ── Rolling rate tracker ──────────────────────────────────────────────────────

struct rate_tracker {
    time_point window_start   = steady_clock::now();
    uint64_t   window_packets = 0;
    uint64_t   window_bytes   = 0;
    float      pps            = 0.f;
    float      mbps           = 0.f;

    void add(uint32_t len) noexcept
    {
        ++window_packets;
        window_bytes += len;
        const auto   now = steady_clock::now();
        const float  dt  = std::chrono::duration<float>(now - window_start).count();
        if (dt >= 1.0f) {
            pps            = static_cast<float>(window_packets) / dt;
            mbps           = static_cast<float>(window_bytes * 8) / (dt * 1e6f);
            window_start   = now;
            window_packets = 0;
            window_bytes   = 0;
        }
    }
};

// ── impl ──────────────────────────────────────────────────────────────────────

struct replayer::impl {
    file_reader & reader;
    device &      dev;

    std::atomic<float>   rate             { 1.0f };
    std::atomic<int>     stats_interval_ms { 200 };
    uint32_t             ring_depth        { 8 };

    std::atomic<bool>    running   { false };
    std::atomic<bool>    paused    { false };
    std::atomic<bool>    stop_req  { false };

    // Shared counters — written by sender thread, read by stats timer
    struct alignas(64) {
        std::atomic<uint64_t> packets_sent    { 0 };
        std::atomic<uint64_t> packets_skipped { 0 };
        std::atomic<uint64_t> bytes_sent      { 0 };
    } counters;

    rate_tracker tracker;
    time_point   start_time;

    std::unique_ptr<std::thread>            reader_thread;
    std::unique_ptr<std::thread>            sender_thread;
    std::unique_ptr<nx::asio::steady_timer> stats_timer;

    std::shared_ptr<std::atomic_bool> alive;
    nx::core::thread *                caller_thread = nullptr;
    replayer *                        self          = nullptr;

    impl(file_reader & r, device & d)
        : reader(r), dev(d)
        , alive(std::make_shared<std::atomic_bool>(false))
    {}
};

// ── replayer ──────────────────────────────────────────────────────────────────

replayer::replayer(file_reader & reader, device & dev, nx::core::object * parent)
    : nx::core::object(parent)
    , impl_(std::make_unique<impl>(reader, dev))
{
    impl_->self = this;
}

replayer::~replayer() { stop(); }

void replayer::set_rate(float rate) noexcept
{
    impl_->rate.store(rate, std::memory_order_relaxed);
}

void replayer::set_stats_interval(int ms) noexcept
{
    impl_->stats_interval_ms.store(ms, std::memory_order_relaxed);
}

void replayer::set_ring_depth(uint32_t depth) noexcept
{
    impl_->ring_depth = depth;
}

bool replayer::is_running() const noexcept { return impl_->running.load(); }
bool replayer::is_paused()  const noexcept { return impl_->paused.load();  }

void replayer::pause()  { impl_->paused.store(true,  std::memory_order_relaxed); }
void replayer::resume() { impl_->paused.store(false, std::memory_order_relaxed); }

void replayer::stop()
{
    *impl_->alive = false;
    impl_->stop_req.store(true, std::memory_order_relaxed);

    if (impl_->stats_timer) impl_->stats_timer->cancel();

    if (impl_->reader_thread && impl_->reader_thread->joinable())
        impl_->reader_thread->join();
    if (impl_->sender_thread && impl_->sender_thread->joinable())
        impl_->sender_thread->join();

    impl_->running.store(false, std::memory_order_relaxed);
}

nx::result<void> replayer::start()
{
    if (impl_->running.load())
        return nx::err::invalid_state("replayer: already running");
    if (!impl_->reader.is_open())
        return nx::err::invalid_state("replayer: file_reader not open");
    if (!impl_->dev.is_open())
        return nx::err::invalid_state("replayer: device not open");

    auto * t = get_thread();
    if (!t)
        return nx::err::invalid_state("replayer: not assigned to a thread");

    impl_->caller_thread = t;
    impl_->stop_req.store(false, std::memory_order_relaxed);
    impl_->paused.store(false,   std::memory_order_relaxed);
    impl_->counters.packets_sent.store(0,    std::memory_order_relaxed);
    impl_->counters.packets_skipped.store(0, std::memory_order_relaxed);
    impl_->counters.bytes_sent.store(0,      std::memory_order_relaxed);
    impl_->start_time = steady_clock::now();
    *impl_->alive = true;
    impl_->running.store(true, std::memory_order_relaxed);

    impl_->stats_timer = std::make_unique<nx::asio::steady_timer>(t->context());

    const uint32_t depth = impl_->ring_depth;
    auto pool = std::make_shared<batch_pool>(depth);
    auto ring = std::make_shared<spsc_ring>(depth);

    // ── Reader thread ──────────────────────────────────────────────────────
    impl_->reader_thread = std::make_unique<std::thread>(
        [p = impl_.get(), pool, ring]()
    {
        while (!p->stop_req.load(std::memory_order_relaxed)) {
            packet_batch * batch = nullptr;
            while (!(batch = pool->acquire())) {
                if (p->stop_req.load(std::memory_order_relaxed)) return;
                NX_CPU_PAUSE();
            }

            auto res = p->reader.read_batch(*batch);
            if (!res || res.value() == 0) {
                batch->count = 0; // EOF sentinel
                while (!ring->push(batch)) NX_CPU_PAUSE();
                return;
            }

            while (!ring->push(batch)) {
                if (p->stop_req.load(std::memory_order_relaxed)) {
                    pool->release(batch);
                    return;
                }
                NX_CPU_PAUSE();
            }
        }
    });

    // ── Sender thread ──────────────────────────────────────────────────────
    impl_->sender_thread = std::make_unique<std::thread>(
        [p = impl_.get(), pool, ring]()
    {
        bool       first_pkt   = true;
        int64_t    first_ts_us = 0;
        time_point wall_start;

        auto send_finished = [&]() {
            auto alive = p->alive;
            p->running.store(false, std::memory_order_relaxed);
            p->caller_thread->post([p, alive]() {
                if (!*alive) return;
                p->self->finished();
            });
        };

        while (!p->stop_req.load(std::memory_order_relaxed)) {

            while (p->paused.load(std::memory_order_relaxed)) {
                if (p->stop_req.load(std::memory_order_relaxed)) {
                    send_finished(); return;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
                first_pkt = true; // reset timing anchor after pause
            }

            packet_batch * batch = ring->pop();
            if (!batch) { NX_CPU_PAUSE(); continue; }

            if (batch->count == 0) { // EOF sentinel
                pool->release(batch);
                break;
            }

            const float rate = p->rate.load(std::memory_order_relaxed);

            for (uint32_t i = 0; i < batch->count; ++i) {
                if (p->stop_req.load(std::memory_order_relaxed)) {
                    pool->release(batch);
                    send_finished(); return;
                }
                while (p->paused.load(std::memory_order_relaxed)) {
                    if (p->stop_req.load(std::memory_order_relaxed)) {
                        pool->release(batch); send_finished(); return;
                    }
                    std::this_thread::sleep_for(std::chrono::milliseconds(5));
                    first_pkt = true;
                }

                const auto & pv = batch->packets[i];

                // Timing
                if (rate > 0.0f) {
                    const int64_t pkt_us = ts_to_us(pv.ts);
                    if (first_pkt) {
                        first_ts_us = pkt_us;
                        wall_start  = steady_clock::now();
                        first_pkt   = false;
                    } else {
                        const int64_t cap_delta  = pkt_us - first_ts_us;
                        const int64_t wall_delta = static_cast<int64_t>(
                            static_cast<float>(cap_delta) / rate);
                        const time_point target = wall_start + micros(wall_delta);
                        if (steady_clock::now() < target)
                            sleep_until_tp(target);
                    }
                }

                // Inject
                if (p->dev.inject(pv.data)) {
                    p->counters.packets_sent.fetch_add(1, std::memory_order_relaxed);
                    p->counters.bytes_sent.fetch_add(pv.data.size(), std::memory_order_relaxed);
                    p->tracker.add(static_cast<uint32_t>(pv.data.size()));
                } else {
                    p->counters.packets_skipped.fetch_add(1, std::memory_order_relaxed);
                }
            }

            pool->release(batch);
        }

        send_finished();
    });

    _arm_stats_timer();
    return {};
}

void replayer::_arm_stats_timer()
{
    auto alive = impl_->alive;
    auto * p   = impl_.get();

    impl_->stats_timer->async_wait(
        std::chrono::milliseconds(impl_->stats_interval_ms.load()),
        [this, p, alive]()
        {
            if (!*alive) return;

            replay_stats s;
            s.packets_sent    = p->counters.packets_sent.load(std::memory_order_relaxed);
            s.packets_skipped = p->counters.packets_skipped.load(std::memory_order_relaxed);
            s.bytes_sent      = p->counters.bytes_sent.load(std::memory_order_relaxed);
            s.elapsed_us      = static_cast<uint64_t>(
                std::chrono::duration_cast<micros>(
                    steady_clock::now() - p->start_time).count());
            s.pps  = p->tracker.pps;
            s.mbps = p->tracker.mbps;

            stats_updated(s);

            if (p->running.load(std::memory_order_relaxed))
                _arm_stats_timer();
        });
}

} // namespace nx::network::pcap
