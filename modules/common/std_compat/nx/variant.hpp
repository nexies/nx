#pragma once

#include <nx/common/platform/standard_defs.hpp>

// ── nx::variant / nx::in_place_type_t ────────────────────────────────────────
// C++17+: thin wrappers around std::variant / std::in_place_type_t.
// C++11/14: minimal 2-type variant used internally by nx::basic_result.
//
// NOTE: the C++11 variant only supports exactly 2 types.  The two types must be
// distinct (undefined behaviour if T1 == T2, matching std::variant's restriction
// on get<T> when T is not unique).

#if defined(NX_CPP17) || defined(NX_CPP20) || defined(NX_CPP23)

#include <variant>
namespace nx {
    using std::variant;
    using std::get;
    template<typename T> using in_place_type_t = std::in_place_type_t<T>;
}

#else

#include <new>
#include <type_traits>
#include <stdexcept>
#include <utility>

namespace nx {

// ── in_place_type_t ───────────────────────────────────────────────────────────
template<typename T>
struct in_place_type_t {};

// ── variant<T1, T2> ───────────────────────────────────────────────────────────

namespace detail {

template<typename T, typename T1, typename T2>
struct variant_get_helper {};

} // namespace detail

template<typename T1, typename T2>
class variant {
public:
    template<typename... Args>
    explicit variant(in_place_type_t<T1>, Args &&... args) : index_(0) {
        new (ptr1()) T1(std::forward<Args>(args)...);
    }

    template<typename... Args>
    explicit variant(in_place_type_t<T2>, Args &&... args) : index_(1) {
        new (ptr2()) T2(std::forward<Args>(args)...);
    }

    variant(const variant & o) : index_(o.index_) {
        if (index_ == 0) new (ptr1()) T1(*o.ptr1());
        else             new (ptr2()) T2(*o.ptr2());
    }
    variant(variant && o) noexcept : index_(o.index_) {
        if (index_ == 0) new (ptr1()) T1(std::move(*o.ptr1()));
        else             new (ptr2()) T2(std::move(*o.ptr2()));
    }

    variant & operator=(const variant & o) {
        if (this != &o) {
            destroy();
            index_ = o.index_;
            if (index_ == 0) new (ptr1()) T1(*o.ptr1());
            else             new (ptr2()) T2(*o.ptr2());
        }
        return *this;
    }
    variant & operator=(variant && o) noexcept {
        if (this != &o) {
            destroy();
            index_ = o.index_;
            if (index_ == 0) new (ptr1()) T1(std::move(*o.ptr1()));
            else             new (ptr2()) T2(std::move(*o.ptr2()));
        }
        return *this;
    }

    ~variant() { destroy(); }

    int index() const noexcept { return index_; }

    template<typename T, typename U1, typename U2>
    friend struct detail::variant_get_helper;

private:
    typename std::aligned_union<1, T1, T2>::type storage_;
    int index_;

    T1 *       ptr1()       noexcept { return reinterpret_cast<T1 *>(&storage_); }
    const T1 * ptr1() const noexcept { return reinterpret_cast<const T1 *>(&storage_); }
    T2 *       ptr2()       noexcept { return reinterpret_cast<T2 *>(&storage_); }
    const T2 * ptr2() const noexcept { return reinterpret_cast<const T2 *>(&storage_); }

    void destroy() noexcept {
        if (index_ == 0) ptr1()->~T1();
        else             ptr2()->~T2();
    }
};

// ── get<T> helpers ────────────────────────────────────────────────────────────

namespace detail {

// Specialisation: get the first type (T == T1)
template<typename T1, typename T2>
struct variant_get_helper<T1, T1, T2> {
    static T1 & call(variant<T1, T2> & v) {
        if (v.index_ != 0) throw std::runtime_error("nx::get: bad variant access");
        return *v.ptr1();
    }
    static const T1 & call(const variant<T1, T2> & v) {
        if (v.index_ != 0) throw std::runtime_error("nx::get: bad variant access");
        return *v.ptr1();
    }
};

// Specialisation: get the second type (T == T2)
// Only compiled when T1 != T2; if T1==T2 the specialisation above wins and this
// would cause an ambiguity -- but such a variant<T, T> is not supported anyway.
template<typename T1, typename T2>
struct variant_get_helper<T2, T1, T2> {
    static T2 & call(variant<T1, T2> & v) {
        if (v.index_ != 1) throw std::runtime_error("nx::get: bad variant access");
        return *v.ptr2();
    }
    static const T2 & call(const variant<T1, T2> & v) {
        if (v.index_ != 1) throw std::runtime_error("nx::get: bad variant access");
        return *v.ptr2();
    }
};

} // namespace detail

template<typename T, typename T1, typename T2>
inline T & get(variant<T1, T2> & v) {
    return detail::variant_get_helper<T, T1, T2>::call(v);
}

template<typename T, typename T1, typename T2>
inline const T & get(const variant<T1, T2> & v) {
    return detail::variant_get_helper<T, T1, T2>::call(v);
}

} // namespace nx

#endif // NX_CPP17+
