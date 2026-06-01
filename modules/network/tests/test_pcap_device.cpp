#include <nx/network/pcap/device.hpp>

#include <catch2/catch_test_macros.hpp>

using namespace nx::network::pcap;

// ── Construction ──────────────────────────────────────────────────────────────

TEST_CASE("device: default constructed is not open", "[pcap][device]")
{
    device d;
    REQUIRE_FALSE(d.is_open());
}

TEST_CASE("device: info() on closed device is safe", "[pcap][device]")
{
    device d;
    const auto & inf = d.info();
    REQUIRE(inf.name.empty());
}

// ── Error cases (no privileges needed) ───────────────────────────────────────

TEST_CASE("device: open nonexistent interface returns error", "[pcap][device]")
{
    device d;
    auto res = d.open("nx_nonexistent_iface_xyz");
    REQUIRE_FALSE(res);
}

TEST_CASE("device: close on not-open device is safe", "[pcap][device]")
{
    device d;
    REQUIRE_NOTHROW(d.close());
    REQUIRE_NOTHROW(d.close()); // double close also safe
}

TEST_CASE("device: inject on closed device returns error", "[pcap][device]")
{
    device d;
    const uint8_t buf[] = { 0x00, 0x11, 0x22 };
    auto res = d.inject({ buf, sizeof(buf) });
    REQUIRE_FALSE(res);
}

TEST_CASE("device: inject_batch on closed device returns 0", "[pcap][device]")
{
    device d;
    auto batch = std::make_unique<packet_batch>();
    // fill one dummy entry
    static const uint8_t dummy[] = { 0xFF };
    batch->packets[0].data = { dummy, 1 };
    batch->count = 1;

    REQUIRE(d.inject_batch(*batch) == 0);
}

// ── Interface enumeration (read-only, no privileges required on most OSes) ───

TEST_CASE("device: list_interfaces returns a result", "[pcap][device]")
{
    auto res = device::list_interfaces();
    // If the call fails (e.g. no pcap permissions at all), we at least get a
    // well-formed error.  On Linux without root it may still succeed.
    if (res) {
        REQUIRE_FALSE(res.value().empty());
        // Every entry must have a non-empty name
        for (const auto & inf : res.value())
            REQUIRE_FALSE(inf.name.empty());
    } else {
        // Acceptable: some systems require root even for enumeration
        SUCCEED("list_interfaces() requires elevated privileges on this system");
    }
}

TEST_CASE("device: interface_info fields are consistent", "[pcap][device]")
{
    auto res = device::list_interfaces();
    if (!res) return; // skip if no permissions

    for (const auto & inf : res.value()) {
        // name must be printable ASCII (at least non-empty)
        REQUIRE_FALSE(inf.name.empty());
        // description may be empty, loopback is a bool — just check it doesn't crash
        (void)inf.description;
        (void)inf.loopback;
    }
}
