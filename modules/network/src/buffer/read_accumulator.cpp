#include <nx/network/buffer/read_accumulator.hpp>

#include <cstring>

namespace nx::network {

read_accumulator::read_accumulator(nx::core::object * parent)
    : nx::core::object(parent)
{}

void read_accumulator::feed(nx::span<const char> data)
{
    if (data.empty()) return;

    auto dest = buf_.prepare(data.size());
    std::memcpy(dest.data(), data.data(), data.size());
    buf_.commit(data.size());

    NX_EMIT(data_available)
}

nx::span<const char> read_accumulator::buffered() const noexcept
{
    return buf_.data();
}

bool read_accumulator::empty() const noexcept
{
    return buf_.empty();
}

void read_accumulator::consume(std::size_t n)
{
    buf_.consume(n);
}

std::optional<nx::span<const char>>
read_accumulator::try_read_exactly(std::size_t n) const noexcept
{
    return buf_.try_read_exactly(n);
}

std::optional<nx::span<const char>>
read_accumulator::try_read_until(char delim) const noexcept
{
    return buf_.try_read_until(delim);
}

std::optional<nx::span<const char>>
read_accumulator::try_read_until(std::string_view delim) const noexcept
{
    return buf_.try_read_until(delim);
}

} // namespace nx::network
