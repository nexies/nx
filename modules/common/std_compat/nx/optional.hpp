#pragma once

#include <nx/common/platform/standard_defs.hpp>

// ── nx::optional ──────────────────────────────────────────────────────────────
// C++17+: alias to std::optional, std::nullopt, std::in_place.
// C++11/14: minimal header-only implementation with the same interface.

#if defined(NX_CPP17) || defined(NX_CPP20) || defined(NX_CPP23)

#include <optional>
namespace nx {
    using std::optional;
    using std::nullopt_t;
    using std::nullopt;
    using std::in_place_t;
    using std::in_place;
}

#else

#include <new>
#include <type_traits>
#include <stdexcept>
#include <utility>

namespace nx {

struct nullopt_t {
    explicit constexpr nullopt_t(int) noexcept {}
};
constexpr nullopt_t nullopt{0};

struct in_place_t {};
constexpr in_place_t in_place;

template<typename T>
class optional {
    typename std::aligned_storage<sizeof(T), alignof(T)>::type storage_;
    bool has_;

    T *       ptr()       noexcept { return reinterpret_cast<T *>(&storage_); }
    const T * ptr() const noexcept { return reinterpret_cast<const T *>(&storage_); }

public:
    using value_type = T;

    constexpr optional() noexcept : has_(false) {}
    constexpr optional(nullopt_t) noexcept : has_(false) {}

    optional(const T & v) : has_(true) { new (&storage_) T(v); }
    optional(T && v)       : has_(true) { new (&storage_) T(std::move(v)); }

    optional(const optional & o) : has_(o.has_) {
        if (has_) new (&storage_) T(*o.ptr());
    }
    optional(optional && o) noexcept : has_(o.has_) {
        if (has_) { new (&storage_) T(std::move(*o.ptr())); o.reset(); }
    }

    optional & operator=(nullopt_t) noexcept { reset(); return *this; }
    optional & operator=(const optional & o) {
        if (this != &o) { reset(); if (o.has_) { new (&storage_) T(*o.ptr()); has_ = true; } }
        return *this;
    }
    optional & operator=(optional && o) noexcept {
        if (this != &o) { reset(); if (o.has_) { new (&storage_) T(std::move(*o.ptr())); has_ = true; o.reset(); } }
        return *this;
    }
    optional & operator=(const T & v) { reset(); new (&storage_) T(v); has_ = true; return *this; }
    optional & operator=(T && v)       { reset(); new (&storage_) T(std::move(v)); has_ = true; return *this; }

    ~optional() { reset(); }

    void reset() noexcept {
        if (has_) { ptr()->~T(); has_ = false; }
    }

    bool has_value() const noexcept { return has_; }
    explicit operator bool() const noexcept { return has_; }

    T &       value()       { if (!has_) throw std::runtime_error("nx::optional: bad access"); return *ptr(); }
    const T & value() const { if (!has_) throw std::runtime_error("nx::optional: bad access"); return *ptr(); }

    T &       operator*()       noexcept { return *ptr(); }
    const T & operator*() const noexcept { return *ptr(); }
    T *       operator->()       noexcept { return ptr(); }
    const T * operator->() const noexcept { return ptr(); }
};

} // namespace nx

#endif // NX_CPP17+
