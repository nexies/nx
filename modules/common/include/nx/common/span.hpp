#pragma once

#include <nx/common/platform/standard_defs.hpp>

// ── nx::span ──────────────────────────────────────────────────────────────────
//
// In C++20 and later: thin alias over std::span<T> (dynamic extent only).
// In C++17: a minimal, header-only implementation with the same interface.

#if defined(NX_CPP20)

#  include <span>
namespace nx {
    template<typename T>
    using span = std::span<T>;
    inline constexpr std::size_t dynamic_extent = std::dynamic_extent;
} // namespace nx

#else // C++17

#  include <cstddef>
#  include <type_traits>

namespace nx {

inline constexpr std::size_t dynamic_extent = std::size_t(-1);

template<typename T>
class span {
public:
    using element_type    = T;
    using value_type      = std::remove_cv_t<T>;
    using size_type       = std::size_t;
    using difference_type = std::ptrdiff_t;
    using pointer         = T *;
    using reference       = T &;
    using iterator        = pointer;

    constexpr span() noexcept : data_(nullptr), size_(0) {}

    constexpr span(pointer p, size_type n) noexcept : data_(p), size_(n) {}

    constexpr span(pointer first, pointer last) noexcept
        : data_(first), size_(static_cast<size_type>(last - first)) {}

    template<std::size_t N>
    constexpr span(T (&arr)[N]) noexcept : data_(arr), size_(N) {}

    // Construct from any contiguous container exposing .data() / .size().
    template<typename C,
             typename = std::enable_if_t<
                 !std::is_same<std::remove_cv_t<C>, span>::value &&
                 std::is_convertible<decltype(std::declval<C &>().data()), pointer>::value>>
    constexpr span(C & c) noexcept : data_(c.data()), size_(c.size()) {}

    constexpr pointer   data()  const noexcept { return data_; }
    constexpr size_type size()  const noexcept { return size_; }
    constexpr bool      empty() const noexcept { return size_ == 0; }

    constexpr reference operator[](size_type i) const noexcept { return data_[i]; }

    constexpr iterator begin() const noexcept { return data_; }
    constexpr iterator end()   const noexcept { return data_ + size_; }

    constexpr span subspan(size_type offset,
                            size_type count = dynamic_extent) const noexcept
    {
        const size_type n = (count == dynamic_extent) ? size_ - offset : count;
        return { data_ + offset, n };
    }

    constexpr span first(size_type n) const noexcept { return { data_, n }; }
    constexpr span last (size_type n) const noexcept { return { data_ + size_ - n, n }; }

private:
    pointer   data_;
    size_type size_;
};

} // namespace nx

#endif // NX_CPP20
