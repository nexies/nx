#include <nx/network/buffer/read_buffer.hpp>

#include <algorithm>
#include <cassert>
#include <cstring>
#include <stdexcept>

namespace nx::network {

read_buffer::read_buffer(std::size_t initial_capacity)
    : buf_(initial_capacity)
{}

nx::span<const char> read_buffer::data() const noexcept
{
    return { buf_.data() + head_, tail_ - head_ };
}

std::size_t read_buffer::size()  const noexcept { return tail_ - head_; }
bool        read_buffer::empty() const noexcept { return head_ == tail_; }

void read_buffer::consume(std::size_t n)
{
    head_ += n;
    if (head_ >= tail_) {
        head_ = tail_ = 0;
    } else if (head_ > buf_.size() / 2) {
        _compact();
    }
}

std::optional<nx::span<const char>>
read_buffer::try_read_exactly(std::size_t n) const noexcept
{
    if (size() < n) return std::nullopt;
    return data().subspan(0, n);
}

std::optional<nx::span<const char>>
read_buffer::try_read_until(char delim) const noexcept
{
    const auto d = data();
    for (std::size_t i = 0; i < d.size(); ++i) {
        if (d[i] == delim)
            return d.subspan(0, i + 1);
    }
    return std::nullopt;
}

std::optional<nx::span<const char>>
read_buffer::try_read_until(std::string_view delim) const noexcept
{
    if (delim.empty())
        return data().subspan(0, 0);

    const auto d = data();
    const std::string_view view(d.data(), d.size());
    const auto pos = view.find(delim);
    if (pos == std::string_view::npos) return std::nullopt;
    return d.subspan(0, pos + delim.size());
}

nx::span<char> read_buffer::prepare(std::size_t hint)
{
    const std::size_t avail = buf_.size() - tail_;
    if (avail < hint) {
        const std::size_t needed = size() + hint;
        if (needed <= buf_.size())
            _compact();
        else
            _grow(needed);
    }
    return { buf_.data() + tail_, buf_.size() - tail_ };
}

void read_buffer::commit(std::size_t n)
{
    tail_ += n;
}

void read_buffer::clear() noexcept
{
    head_ = tail_ = 0;
}

std::size_t read_buffer::capacity() const noexcept { return buf_.size(); }

void read_buffer::_compact()
{
    const std::size_t len = tail_ - head_;
    std::memmove(buf_.data(), buf_.data() + head_, len);
    head_ = 0;
    tail_ = len;
}

void read_buffer::_grow(std::size_t min_capacity)
{
    std::size_t new_cap = buf_.size() * 2;
    if (new_cap < min_capacity) new_cap = min_capacity * 2;

    const std::size_t len = tail_ - head_;
    std::vector<char> new_buf(new_cap);
    std::memcpy(new_buf.data(), buf_.data() + head_, len);
    buf_  = std::move(new_buf);
    head_ = 0;
    tail_ = len;
}

} // namespace nx::network
