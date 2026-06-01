#pragma once

#include <nx/network/pcap/packet.hpp>
#include <nx/common/types/result.hpp>

#include <cstdint>
#include <memory>
#include <string>
#include <string_view>

namespace nx::network::pcap {

// ── link_type ─────────────────────────────────────────────────────────────────

enum class link_type : uint32_t {
    ethernet = 1,
    raw_ip   = 101,
    ipv4     = 228,
    ipv6     = 229,
    other    = 0xFFFF,
};

// ── file_info ─────────────────────────────────────────────────────────────────

struct file_info {
    std::string filename;
    link_type   datalink      = link_type::other;
    uint32_t    datalink_raw  = 0;    // raw DLT_ value from libpcap
    uint32_t    snaplen       = 0;    // max capture length in the file
    bool        nanosecond_ts = false;
};

// ── file_reader ───────────────────────────────────────────────────────────────
//
// Streaming reader for pcap / pcapng capture files (backed by libpcap).
//
// Usage:
//   file_reader r;
//   r.open("capture.pcap").value();
//
//   auto batch = std::make_unique<packet_batch>();
//   while (true) {
//       auto n = r.read_batch(*batch).value();
//       if (n == 0) break;  // EOF
//       for (uint32_t i = 0; i < batch->count; ++i)
//           process(batch->packets[i]);
//   }
//
// Not thread-safe: drive from a single thread.

class file_reader {
public:
    file_reader();
    ~file_reader();

    file_reader(const file_reader &)             = delete;
    file_reader & operator=(const file_reader &) = delete;
    file_reader(file_reader &&)                  = delete;
    file_reader & operator=(file_reader &&)      = delete;

    // Open a pcap or pcapng capture file.
    nx::result<void> open(std::string_view path);

    void close();

    [[nodiscard]] bool             is_open() const noexcept;
    [[nodiscard]] const file_info& info()    const noexcept;

    // Fill batch with the next up to packet_batch::capacity packets.
    //
    // Returns the number of packets placed in batch:
    //   > 0  — packets are ready in batch
    //   = 0  — end of file (batch is empty; the reader remains open)
    //
    // Packets larger than packet_batch::max_packet_len are silently skipped.
    [[nodiscard]] nx::result<uint32_t> read_batch(packet_batch & batch);

    // Seek back to the first packet by reopening the file.
    nx::result<void> rewind();

private:
    struct impl;
    std::unique_ptr<impl> impl_;
};

} // namespace nx::network::pcap
