#include <nx/network/pcap/file_reader.hpp>

#include <pcap/pcap.h>

#include <cstring>

namespace nx::network::pcap {

// ── helpers ───────────────────────────────────────────────────────────────────

static link_type classify_dlt(int dlt) noexcept
{
    switch (dlt) {
        case 1:   return link_type::ethernet;
        case 101: return link_type::raw_ip;
        case 228: return link_type::ipv4;
        case 229: return link_type::ipv6;
        default:  return link_type::other;
    }
}

// ── impl ──────────────────────────────────────────────────────────────────────

struct file_reader::impl {
    pcap_t *  handle = nullptr;
    file_info info;
};

// ── file_reader ───────────────────────────────────────────────────────────────

file_reader::file_reader()  = default;
file_reader::~file_reader() { close(); }

nx::result<void> file_reader::open(std::string_view path)
{
    close();

    char errbuf[PCAP_ERRBUF_SIZE] = {};
    const std::string p(path);

    pcap_t * h = ::pcap_open_offline(p.c_str(), errbuf);
    if (!h)
        return nx::err::runtime_error(errbuf);

    impl_ = std::make_unique<impl>();
    impl_->handle = h;

    const int dlt             = ::pcap_datalink(h);
    impl_->info.filename      = p;
    impl_->info.datalink_raw  = static_cast<uint32_t>(dlt);
    impl_->info.datalink      = classify_dlt(dlt);
    impl_->info.snaplen       = static_cast<uint32_t>(::pcap_snapshot(h));
    impl_->info.nanosecond_ts =
        (::pcap_get_tstamp_precision(h) == PCAP_TSTAMP_PRECISION_NANO);

    return {};
}

void file_reader::close()
{
    if (impl_) {
        if (impl_->handle)
            ::pcap_close(impl_->handle);
        impl_.reset();
    }
}

bool file_reader::is_open() const noexcept
{
    return impl_ && impl_->handle;
}

const file_info & file_reader::info() const noexcept
{
    static const file_info empty;
    return impl_ ? impl_->info : empty;
}

nx::result<uint32_t> file_reader::read_batch(packet_batch & batch)
{
    batch.clear();

    if (!is_open())
        return nx::err::invalid_state("file_reader: not open");

    while (!batch.full()) {
        pcap_pkthdr *   hdr  = nullptr;
        const uint8_t * data = nullptr;

        const int r = ::pcap_next_ex(impl_->handle, &hdr, &data);

        if (r == -2) break;    // EOF
        if (r == -1)
            return nx::err::runtime_error(::pcap_geterr(impl_->handle));
        // r == 0: live-capture timeout — cannot occur for offline files

        const uint32_t caplen = hdr->caplen;

        if (caplen > packet_batch::max_packet_len) continue; // skip oversized

        if (!batch.can_fit(caplen)) break; // data buffer full — return current batch

        uint8_t * dst = batch.buf + batch.buf_used;
        std::memcpy(dst, data, caplen);

        auto & pv = batch.packets[batch.count++];
        pv.data   = { dst, caplen };
        pv.ts     = hdr->ts;

        batch.buf_used += caplen;
    }

    return batch.count;
}

nx::result<void> file_reader::rewind()
{
    if (!impl_)
        return nx::err::invalid_state("file_reader: not open");
    return open(impl_->info.filename);
}

} // namespace nx::network::pcap
