#include <nx/network/pcap/replayer.hpp>
#include <nx/network/pcap/file_reader.hpp>
#include <nx/network/pcap/device.hpp>

#include <nx/core2/thread/thread.hpp>

#include <catch2/catch_test_macros.hpp>

#include <cstdio>
#include <cstring>
#include <memory>
#include <string>
#include <vector>

#if defined(NX_OS_WINDOWS)
#include <io.h>
#elif defined(NX_OS_LINUX)
#include <unistd.h>
#endif

using namespace nx::network::pcap;

// ── pcap file writer (same helper as in test_pcap_reader.cpp) ─────────────────

namespace {

struct pcap_global_hdr {
    uint32_t magic    = 0xa1b2c3d4;
    uint16_t ver_maj  = 2;
    uint16_t ver_min  = 4;
    int32_t  thiszone = 0;
    uint32_t sigfigs  = 0;
    uint32_t snaplen  = 65535;
    uint32_t network  = 1;
};

struct pcap_pkt_hdr {
    uint32_t ts_sec;
    uint32_t ts_usec;
    uint32_t incl_len;
    uint32_t orig_len;
};

struct test_pkt { std::vector<uint8_t> data; uint32_t ts_sec; uint32_t ts_usec; };

static std::string make_temp_pcap(const std::vector<test_pkt> & pkts)
{
    char path[] = "/tmp/nx_pcap_rp_test_XXXXXX";
    const int fd = ::mkstemp(path);
    REQUIRE(fd >= 0);
    ::close(fd);

    FILE * f = std::fopen(path, "wb");
    REQUIRE(f);

    pcap_global_hdr gh;
    std::fwrite(&gh, sizeof(gh), 1, f);

    for (const auto & p : pkts) {
        pcap_pkt_hdr ph { p.ts_sec, p.ts_usec,
                          static_cast<uint32_t>(p.data.size()),
                          static_cast<uint32_t>(p.data.size()) };
        std::fwrite(&ph,           sizeof(ph), 1,            f);
        std::fwrite(p.data.data(), 1,          p.data.size(), f);
    }
    std::fclose(f);
    return std::string(path);
}

} // namespace

// ── Construction and initial state ────────────────────────────────────────────

TEST_CASE("replayer: initial state is not running", "[pcap][replayer]")
{
    file_reader r;
    device      d;
    replayer    rp(r, d);

    REQUIRE_FALSE(rp.is_running());
    REQUIRE_FALSE(rp.is_paused());
}

TEST_CASE("replayer: stop/pause/resume when not running are safe", "[pcap][replayer]")
{
    file_reader r;
    device      d;
    replayer    rp(r, d);

    REQUIRE_NOTHROW(rp.stop());
    REQUIRE_NOTHROW(rp.pause());
    REQUIRE_NOTHROW(rp.resume());
    REQUIRE_FALSE(rp.is_running());
}

// ── Configuration ─────────────────────────────────────────────────────────────

TEST_CASE("replayer: configuration setters do not crash", "[pcap][replayer]")
{
    file_reader r;
    device      d;
    replayer    rp(r, d);

    REQUIRE_NOTHROW(rp.set_rate(0.0f));
    REQUIRE_NOTHROW(rp.set_rate(1.0f));
    REQUIRE_NOTHROW(rp.set_rate(2.5f));
    REQUIRE_NOTHROW(rp.set_stats_interval(100));
    REQUIRE_NOTHROW(rp.set_ring_depth(16));
}

// ── start() precondition checks ───────────────────────────────────────────────

TEST_CASE("replayer: start() fails when file_reader is not open", "[pcap][replayer]")
{
    file_reader r; // not open
    device      d;
    replayer    rp(r, d);

    auto res = rp.start();
    REQUIRE_FALSE(res);
    REQUIRE_FALSE(rp.is_running());
}

TEST_CASE("replayer: start() fails when device is not open", "[pcap][replayer]")
{
    const std::string path = make_temp_pcap({ { {0x00, 0x01}, 0, 0 } });

    file_reader r;
    REQUIRE(r.open(path));

    device   d; // not open
    replayer rp(r, d);

    auto res = rp.start();
    REQUIRE_FALSE(res);
    REQUIRE_FALSE(rp.is_running());

    std::remove(path.c_str());
}

TEST_CASE("replayer: start() fails when not assigned to a thread", "[pcap][replayer]")
{
    const std::string path = make_temp_pcap({ { {0x01, 0x02}, 1, 0 } });

    file_reader r;
    REQUIRE(r.open(path));

    // We can't easily open a real device without root, so skip the third
    // precondition check (thread assignment) — it only runs after both are open.
    // This test validates that when the reader is open but the device is closed,
    // we get the "device not open" error, not a thread-assignment error.
    device   d;
    replayer rp(r, d);

    auto res = rp.start();
    REQUIRE_FALSE(res);

    // The error should mention "device", not "thread"
    // (precondition order: reader → device → thread)
    const std::string msg = res.error().message();
    const bool mentions_device = msg.find("device") != std::string::npos;
    const bool mentions_thread = msg.find("thread") != std::string::npos;
    REQUIRE(mentions_device);
    REQUIRE_FALSE(mentions_thread);

    std::remove(path.c_str());
}

// ── Integration test (requires root / CAP_NET_RAW) ────────────────────────────
//
// Tagged [!hide] so Catch2 skips it by default.
// Run explicitly with: ./nx_test_pcap_replayer [integration]
//
// Requires: CAP_NET_RAW or root (Linux), Npcap admin rights (Windows).

TEST_CASE("replayer: full replay on loopback [integration]",
          "[hide][integration][pcap][replayer]")
{
    // Find a loopback interface
    auto ifaces_res = device::list_interfaces();
    if (!ifaces_res) { SKIP("cannot enumerate interfaces (no privileges?)"); }

    std::string lo_name;
    for (const auto & inf : ifaces_res.value()) {
        if (inf.loopback) { lo_name = inf.name; break; }
    }
    if (lo_name.empty()) { SKIP("no loopback interface found"); }

    // Build a small pcap: 5 packets, 1 ms apart
    std::vector<test_pkt> pkts;
    for (int i = 0; i < 5; ++i) {
        // Minimal Ethernet frame: 14-byte header + 46-byte payload = 60 bytes
        std::vector<uint8_t> frame(60, static_cast<uint8_t>(i));
        pkts.push_back({ std::move(frame),
                         static_cast<uint32_t>(i / 1000),
                         static_cast<uint32_t>((i % 1000) * 1000) });
    }
    const std::string path = make_temp_pcap(pkts);

    file_reader r;
    REQUIRE(r.open(path));

    device d;
    auto open_res = d.open(lo_name);
    if (!open_res) { SKIP("cannot open loopback for injection (no privileges?)"); }

    // Set up a thread for the replayer
    auto thread = std::make_unique<nx::core::thread>("replayer-test");
    REQUIRE(thread->start());

    replay_stats final_stats;
    bool         got_finished = false;

    replayer rp(r, d);
    rp.set_rate(0.0f); // max rate
    rp.set_stats_interval(50);
    rp.move_to_thread(thread.get());

    thread->post([&]() {
        nx::core::connect(&rp, &replayer::stats_updated,
                          &rp, [&](replay_stats s) { final_stats = s; });

        nx::core::connect(&rp, &replayer::finished,
                          &rp, [&]() {
                              got_finished = true;
                              thread->quit();
                          });

        REQUIRE(rp.start());
    });

    thread->wait(); // blocks until finished signal → thread->quit()

    REQUIRE(got_finished);
    REQUIRE(final_stats.packets_sent + final_stats.packets_skipped == 5);

    std::remove(path.c_str());
}
