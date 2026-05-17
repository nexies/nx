#include <nx/network/framing/length_prefix_framer.hpp>

namespace nx::network::framing {

length_prefix_framer::length_prefix_framer(read_accumulator * acc,
                                            nx::core::object * parent)
    : nx::core::object(parent)
    , acc_(acc)
{
    nx::core::connect(acc_, &read_accumulator::data_available,
                      this, [this]() { on_data(); });
}

// ── Slot ──────────────────────────────────────────────────────────────────────

void length_prefix_framer::on_data()
{
    while (true) {
        if (!has_header_) {
            auto hdr = acc_->try_read_exactly(header_size);
            if (!hdr) break;

            const auto * b = reinterpret_cast<const uint8_t *>(hdr->data());
            pending_len_ = (std::uint32_t(b[0]) << 24)
                         | (std::uint32_t(b[1]) << 16)
                         | (std::uint32_t(b[2]) <<  8)
                         |  std::uint32_t(b[3]);

            acc_->consume(header_size);
            has_header_ = true;
        }

        auto body = acc_->try_read_exactly(pending_len_);
        if (!body) break;

        NX_EMIT(frame_received, *body)
        acc_->consume(pending_len_);
        has_header_ = false;
    }
}

// ── Static helpers ────────────────────────────────────────────────────────────

std::vector<char> length_prefix_framer::pack(const char * data, std::size_t len)
{
    const auto u = static_cast<std::uint32_t>(len);
    std::vector<char> out(header_size + len);
    out[0] = static_cast<char>(u >> 24);
    out[1] = static_cast<char>(u >> 16);
    out[2] = static_cast<char>(u >>  8);
    out[3] = static_cast<char>(u);
    std::memcpy(out.data() + header_size, data, len);
    return out;
}

std::vector<char> length_prefix_framer::pack(nx::span<const char> data)
{
    return pack(data.data(), data.size());
}

} // namespace nx::network::framing
