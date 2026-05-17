#include <nx/network/framing/delimiter_framer.hpp>

namespace nx::network::framing {

delimiter_framer::delimiter_framer(read_accumulator * acc, char delim,
                                    nx::core::object * parent)
    : delimiter_framer(acc, std::string(1, delim), parent)
{}

delimiter_framer::delimiter_framer(read_accumulator * acc, std::string delim,
                                    nx::core::object * parent)
    : nx::core::object(parent)
    , acc_(acc)
    , delim_(std::move(delim))
{
    nx::core::connect(acc_, &read_accumulator::data_available,
                      this, [this]() { on_data(); });
}

// ── Slot ──────────────────────────────────────────────────────────────────────

void delimiter_framer::on_data()
{
    while (true) {
        auto chunk = acc_->try_read_until(std::string_view(delim_));
        if (!chunk) break;

        const auto frame = chunk->subspan(0, chunk->size() - delim_.size());
        NX_EMIT(frame_received, frame)
        acc_->consume(chunk->size());
    }
}

// ── Static helpers ────────────────────────────────────────────────────────────

std::vector<char> delimiter_framer::pack(const char * data, std::size_t len) const
{
    std::vector<char> out(len + delim_.size());
    std::memcpy(out.data(), data, len);
    std::memcpy(out.data() + len, delim_.data(), delim_.size());
    return out;
}

std::vector<char> delimiter_framer::pack(std::string_view sv) const
{
    return pack(sv.data(), sv.size());
}

} // namespace nx::network::framing
