#include <nx/network/pcap/device.hpp>

#include <pcap/pcap.h>

#include <cstring>

namespace nx::network::pcap {

// ── impl ──────────────────────────────────────────────────────────────────────

struct device::impl {
    pcap_t *       handle = nullptr;
    interface_info info;
};

// ── helpers ───────────────────────────────────────────────────────────────────

static nx::error pcap_err(pcap_t * h, std::string_view ctx)
{
    std::string msg(ctx);
    if (h) {
        msg += ": ";
        msg += ::pcap_geterr(h);
    }
    return nx::err::runtime_error(msg);
}

// ── device ────────────────────────────────────────────────────────────────────

device::device()  = default;
device::~device() { close(); }

nx::result<std::vector<interface_info>> device::list_interfaces()
{
    char errbuf[PCAP_ERRBUF_SIZE] = {};
    pcap_if_t * devs = nullptr;

    if (::pcap_findalldevs(&devs, errbuf) != 0)
        return nx::err::runtime_error(errbuf);

    std::vector<interface_info> result;
    for (pcap_if_t * d = devs; d; d = d->next) {
        interface_info inf;
        inf.name        = d->name ? d->name : "";
        inf.description = d->description ? d->description : "";
        inf.loopback    = (d->flags & PCAP_IF_LOOPBACK) != 0;
        result.push_back(std::move(inf));
    }

    ::pcap_freealldevs(devs);
    return result;
}

nx::result<void> device::open(std::string_view name, int snaplen, int timeout_ms)
{
    close();

    char errbuf[PCAP_ERRBUF_SIZE] = {};
    const std::string n(name);

    pcap_t * h = ::pcap_open_live(n.c_str(), snaplen, 0, timeout_ms, errbuf);
    if (!h)
        return nx::err::runtime_error(errbuf);

    impl_ = std::make_unique<impl>();
    impl_->handle      = h;
    impl_->info.name   = n;
    // errbuf may contain a warning even on success
    if (errbuf[0] != '\0')
        impl_->info.description = errbuf;

    return {};
}

void device::close()
{
    if (impl_) {
        if (impl_->handle)
            ::pcap_close(impl_->handle);
        impl_.reset();
    }
}

bool device::is_open() const noexcept
{
    return impl_ && impl_->handle;
}

const interface_info & device::info() const noexcept
{
    static const interface_info empty;
    return impl_ ? impl_->info : empty;
}

nx::result<void> device::inject(nx::span<const uint8_t> frame)
{
    if (!is_open())
        return nx::err::invalid_state("device: not open");

    // pcap_inject returns the number of bytes written, or -1 on error.
    // On transient buffer-full conditions (ENOBUFS) retry once.
    for (int attempt = 0; attempt < 2; ++attempt) {
        const int r = ::pcap_inject(impl_->handle,
                                    frame.data(),
                                    static_cast<int>(frame.size()));
        if (r >= 0)
            return {};

        const char * err = ::pcap_geterr(impl_->handle);
        // ENOBUFS / EAGAIN — kernel TX queue full, retry after brief yield
        if (attempt == 0 && (std::strstr(err, "ENOBUFS") ||
                             std::strstr(err, "EAGAIN")  ||
                             std::strstr(err, "No buffer space")))
            continue;

        return nx::err::runtime_error(std::string("pcap_inject: ") + err);
    }

    return nx::err::runtime_error("pcap_inject: TX queue full");
}

uint32_t device::inject_batch(const packet_batch & batch, uint32_t count)
{
    if (!is_open()) return 0;

    const uint32_t n = (count < batch.count) ? count : batch.count;
    uint32_t sent = 0;

    for (uint32_t i = 0; i < n; ++i) {
        const auto & pv = batch.packets[i];
        const int r = ::pcap_inject(impl_->handle,
                                    pv.data.data(),
                                    static_cast<int>(pv.data.size()));
        if (r < 0) break;
        ++sent;
    }

    return sent;
}

} // namespace nx::network::pcap
