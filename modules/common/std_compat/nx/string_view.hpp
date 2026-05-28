#pragma once

#include <nx/common/platform/standard_defs.hpp>

// ── nx::string_view ───────────────────────────────────────────────────────────
// C++17+: alias to std::string_view.
// C++11/14: minimal header-only implementation with the same interface.

#if defined(NX_CPP17) || defined(NX_CPP20) || defined(NX_CPP23)

#include <string_view>
namespace nx {
    using string_view = std::string_view;
}

#else

#include <string>
#include <cstring>
#include <stdexcept>

namespace nx {

class string_view {
public:
    using value_type      = char;
    using size_type       = std::size_t;
    using const_pointer   = const char *;
    using const_reference = const char &;
    using const_iterator  = const char *;

    static const size_type npos = static_cast<size_type>(-1);

    // Constructors
    constexpr string_view() noexcept : data_(""), size_(0) {}

    string_view(const char * s) noexcept
        : data_(s ? s : ""), size_(s ? std::strlen(s) : 0) {}

    constexpr string_view(const char * s, size_type len) noexcept
        : data_(s ? s : ""), size_(len) {}

    string_view(const std::string & s) noexcept
        : data_(s.data()), size_(s.size()) {}

    string_view(const string_view &) noexcept            = default;
    string_view & operator=(const string_view &) noexcept = default;

    // Element access
    constexpr const_reference operator[](size_type i) const noexcept { return data_[i]; }
    constexpr const_reference front() const noexcept { return data_[0]; }
    constexpr const_reference back()  const noexcept { return data_[size_ - 1]; }
    constexpr const_pointer   data()  const noexcept { return data_; }

    // Capacity
    constexpr size_type size()   const noexcept { return size_; }
    constexpr size_type length() const noexcept { return size_; }
    constexpr bool      empty()  const noexcept { return size_ == 0; }

    // Iterators
    constexpr const_iterator begin() const noexcept { return data_; }
    constexpr const_iterator end()   const noexcept { return data_ + size_; }

    // Modifiers
    void remove_prefix(size_type n) noexcept { data_ += n; size_ -= n; }
    void remove_suffix(size_type n) noexcept { size_ -= n; }

    // Substring
    string_view substr(size_type pos = 0, size_type len = npos) const {
        if (pos > size_) throw std::out_of_range("nx::string_view::substr");
        const size_type n = (len == npos || pos + len > size_) ? size_ - pos : len;
        return string_view(data_ + pos, n);
    }

    // Search
    size_type find(char c, size_type pos = 0) const noexcept {
        for (size_type i = pos; i < size_; ++i)
            if (data_[i] == c) return i;
        return npos;
    }

    size_type find(const char * s, size_type pos = 0) const noexcept {
        const size_type slen = std::strlen(s);
        if (slen > size_) return npos;
        for (size_type i = pos; i + slen <= size_; ++i)
            if (std::memcmp(data_ + i, s, slen) == 0) return i;
        return npos;
    }

    size_type find(string_view sv, size_type pos = 0) const noexcept {
        if (sv.size_ > size_) return npos;
        for (size_type i = pos; i + sv.size_ <= size_; ++i)
            if (std::memcmp(data_ + i, sv.data_, sv.size_) == 0) return i;
        return npos;
    }

    // Conversion — non-explicit so assignments to std::string work as in C++17
    operator std::string() const { return std::string(data_, size_); }

    // Comparisons
    bool operator==(string_view o) const noexcept {
        return size_ == o.size_ &&
               (size_ == 0 || std::memcmp(data_, o.data_, size_) == 0);
    }
    bool operator!=(string_view o) const noexcept { return !(*this == o); }
    bool operator< (string_view o) const noexcept {
        const size_type n = size_ < o.size_ ? size_ : o.size_;
        const int r = n ? std::memcmp(data_, o.data_, n) : 0;
        return r < 0 || (r == 0 && size_ < o.size_);
    }

private:
    const char * data_;
    size_type    size_;
};

inline bool operator==(const char * s, string_view sv) noexcept { return sv == string_view(s); }
inline bool operator!=(const char * s, string_view sv) noexcept { return sv != string_view(s); }
inline bool operator==(const std::string & s, string_view sv) noexcept { return sv == string_view(s); }
inline bool operator!=(const std::string & s, string_view sv) noexcept { return sv != string_view(s); }

inline std::string & operator+=(std::string & s, string_view sv) {
    return s.append(sv.data(), sv.size());
}

} // namespace nx

#include <ostream>
namespace nx {
inline std::ostream & operator<<(std::ostream & os, string_view sv) {
    return os.write(sv.data(), static_cast<std::streamsize>(sv.size()));
}
} // namespace nx

#endif // NX_CPP17+
