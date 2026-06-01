#include <nx/network/pcap/file_reader.hpp>

#include <catch2/catch_test_macros.hpp>

#include <cstdio>
#include <memory>
#include <cstring>
#include <string>
#include <vector>

#if defined(NX_OS_WINDOWS)
#include <io.h>
#elif defined(NX_OS_LINUX)
#include <unistd.h>
#endif

using namespace nx::network::pcap;

// ── pcap file writer helper ───────────────────────────────────────────────────
//
// Writes a minimal pcap file (microsecond timestamps, DLT_EN10MB) without
// linking libpcap — the format is stable and trivially hand-crafted.

namespace {

// pcap global header (24 bytes, little-endian host byte order)
struct pcap_global_hdr {
    uint32_t magic    = 0xa1b2c3d4; // microsecond precision
    uint16_t ver_maj  = 2;
    uint16_t ver_min  = 4;
    int32_t  thiszone = 0;
    uint32_t sigfigs  = 0;
    uint32_t snaplen  = 65535;
    uint32_t network  = 1; // DLT_EN10MB
};

// pcap per-packet header (16 bytes)
struct pcap_pkt_hdr {
    uint32_t ts_sec;
    uint32_t ts_usec;
    uint32_t incl_len;
    uint32_t orig_len;
};

struct test_packet {
    std::vector<uint8_t> data;
    uint32_t             ts_sec;
    uint32_t             ts_usec;
};



// Writes packets to a temp file and returns its path.
// Caller must std::remove() the path when done.
static std::string make_temp_pcap(const std::vector<test_packet> & pkts,
                                   uint32_t network = 1 /* DLT_EN10MB */)
{
    char path[] = "/tmp/nx_pcap_test_XXXXXX";
    const int fd = ::mkstemp(path);
    REQUIRE(fd >= 0);
    ::close(fd);

    FILE * f = std::fopen(path, "wb");
    REQUIRE(f != nullptr);

    pcap_global_hdr ghdr;
    ghdr.network = network;
    std::fwrite(&ghdr, sizeof(ghdr), 1, f);

    for (const auto & p : pkts) {
        pcap_pkt_hdr phdr;
        phdr.ts_sec  = p.ts_sec;
        phdr.ts_usec = p.ts_usec;
        phdr.incl_len = static_cast<uint32_t>(p.data.size());
        phdr.orig_len = static_cast<uint32_t>(p.data.size());
        std::fwrite(&phdr,         sizeof(phdr),     1,          f);
        std::fwrite(p.data.data(), 1, p.data.size(), f);
    }

    std::fclose(f);
    return std::string(path);
}

} // namespace

// ── Tests ─────────────────────────────────────────────────────────────────────

TEST_CASE("file_reader: default constructed is not open", "[pcap]")
{
    file_reader r;
    REQUIRE_FALSE(r.is_open());
}

TEST_CASE("file_reader: open nonexistent file returns error", "[pcap]")
{
    file_reader r;
    auto res = r.open("/nonexistent/path/capture.pcap");
    REQUIRE_FALSE(res);
}

TEST_CASE("file_reader: open valid pcap and check metadata", "[pcap]")
{
    const std::string path = make_temp_pcap({});

    file_reader r;
    REQUIRE(r.open(path));
    REQUIRE(r.is_open());

    const auto & inf = r.info();
    REQUIRE(inf.filename == path);
    REQUIRE(inf.datalink == link_type::ethernet);
    REQUIRE(inf.datalink_raw == 1);
    REQUIRE(inf.snaplen == 65535);

    std::remove(path.c_str());
}

TEST_CASE("file_reader: empty file returns 0 from read_batch", "[pcap]")
{
    const std::string path = make_temp_pcap({});

    file_reader r;
    REQUIRE(r.open(path));

    auto batch = std::make_unique<packet_batch>();
    auto res = r.read_batch(*batch);
    REQUIRE(res);
    REQUIRE(res.value() == 0);
    REQUIRE(batch->count == 0);

    std::remove(path.c_str());
}

TEST_CASE("file_reader: read packets preserves data and timestamps", "[pcap]")
{
    const std::vector<test_packet> input = {
        { {0x00, 0x11, 0x22, 0x33, 0x44, 0x55}, 1000, 0   },
        { {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF}, 1000, 500 },
        { {0x01, 0x02, 0x03, 0x04},              1001, 0   },
    };

    const std::string path = make_temp_pcap(input);

    file_reader r;
    REQUIRE(r.open(path));

    auto batch = std::make_unique<packet_batch>();
    auto res = r.read_batch(*batch);
    REQUIRE(res);
    REQUIRE(res.value() == 3);
    REQUIRE(batch->count == 3);

    REQUIRE(batch->packets[0].data.size() == 6);
    REQUIRE(batch->packets[0].data[0] == 0x00);
    REQUIRE(batch->packets[0].data[5] == 0x55);
    REQUIRE(batch->packets[0].ts.tv_sec  == 1000);
    REQUIRE(batch->packets[0].ts.tv_usec == 0);

    REQUIRE(batch->packets[1].data[0] == 0xAA);
    REQUIRE(batch->packets[1].data[5] == 0xFF);
    REQUIRE(batch->packets[1].ts.tv_usec == 500);

    REQUIRE(batch->packets[2].data.size() == 4);
    REQUIRE(batch->packets[2].ts.tv_sec == 1001);

    // EOF on second call
    auto res2 = r.read_batch(*batch);
    REQUIRE(res2);
    REQUIRE(res2.value() == 0);

    std::remove(path.c_str());
}

TEST_CASE("file_reader: close makes reader not open", "[pcap]")
{
    const std::string path = make_temp_pcap({});

    file_reader r;
    REQUIRE(r.open(path));
    REQUIRE(r.is_open());

    r.close();
    REQUIRE_FALSE(r.is_open());

    std::remove(path.c_str());
}

TEST_CASE("file_reader: read_batch on closed reader returns error", "[pcap]")
{
    file_reader r;
    auto batch = std::make_unique<packet_batch>();
    auto res = r.read_batch(*batch);
    REQUIRE_FALSE(res);
}

TEST_CASE("file_reader: rewind restarts packet stream", "[pcap]")
{
    const std::vector<test_packet> input = {
        { {0xAA, 0xBB}, 1, 0 },
        { {0xCC, 0xDD}, 2, 0 },
    };

    const std::string path = make_temp_pcap(input);

    file_reader r;
    REQUIRE(r.open(path));

    auto batch = std::make_unique<packet_batch>();
    REQUIRE(r.read_batch(*batch).value() == 2);
    REQUIRE(r.read_batch(*batch).value() == 0); // EOF

    REQUIRE(r.rewind());                         // back to start
    REQUIRE(r.read_batch(*batch).value() == 2);
    REQUIRE(batch->packets[0].data[0] == 0xAA);

    std::remove(path.c_str());
}

TEST_CASE("file_reader: batch splits across multiple reads", "[pcap]")
{
    // Write capacity+1 packets so we need two read_batch calls
    const uint32_t total = packet_batch::capacity + 1;
    std::vector<test_packet> input;
    input.reserve(total);
    for (uint32_t i = 0; i < total; ++i)
        input.push_back({ {static_cast<uint8_t>(i)}, i, 0 });

    const std::string path = make_temp_pcap(input);

    file_reader r;
    REQUIRE(r.open(path));

    auto batch = std::make_unique<packet_batch>();

    auto res1 = r.read_batch(*batch);
    REQUIRE(res1);
    REQUIRE(res1.value() == packet_batch::capacity);
    REQUIRE(batch->packets[0].data[0] == 0);

    auto res2 = r.read_batch(*batch);
    REQUIRE(res2);
    REQUIRE(res2.value() == 1);
    REQUIRE(batch->packets[0].data[0] == static_cast<uint8_t>(packet_batch::capacity));

    REQUIRE(r.read_batch(*batch).value() == 0); // EOF

    std::remove(path.c_str());
}
