#pragma once

#include <nx/network/pcap/packet.hpp>
#include <nx/common/types/result.hpp>

#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace nx::network::pcap {

// ── interface_info ────────────────────────────────────────────────────────────

struct interface_info {
    std::string name;         // "eth0", "\Device\NPF_{GUID}"
    std::string description;  // human-readable, may be empty on Linux
    bool        loopback = false;
};

// ── device ────────────────────────────────────────────────────────────────────
//
// Wraps a live pcap handle for packet injection.
//
// Usage:
//   auto ifaces = device::list_interfaces().value();
//   device dev;
//   dev.open(ifaces[0].name).value();
//   dev.inject(frame).value();
//
// inject() and inject_batch() require elevated privileges (root / CAP_NET_RAW
// on Linux, Npcap with admin rights on Windows).
//
// Not thread-safe: drive from a single thread (the sender thread).

class device {
public:
    device();
    ~device();

    device(const device &)             = delete;
    device & operator=(const device &) = delete;
    device(device &&)                  = delete;
    device & operator=(device &&)      = delete;

    // Enumerate network interfaces available for packet capture / injection.
    [[nodiscard]] static nx::result<std::vector<interface_info>> list_interfaces();

    // Open a live interface.  snaplen is the capture snapshot length (unused
    // for injection-only use, but required by pcap_open_live).
    nx::result<void> open(std::string_view name,
                          int snaplen    = 65535,
                          int timeout_ms = 1);

    void close();

    [[nodiscard]] bool                   is_open() const noexcept;
    [[nodiscard]] const interface_info & info()    const noexcept;

    // Inject a single link-layer frame.
    // Returns an error if the write fails; EAGAIN / ENOBUFS are retried once.
    [[nodiscard]] nx::result<void> inject(nx::span<const uint8_t> frame);

    // Inject all packets in [0, count) from batch.
    // Returns the number of packets successfully injected.
    uint32_t inject_batch(const packet_batch & batch,
                          uint32_t             count = packet_batch::capacity);

private:
    struct impl;
    std::unique_ptr<impl> impl_;
};

} // namespace nx::network::pcap
