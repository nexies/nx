//
// function_id — compile-time/runtime unique identifier for function pointers.
// Used by the signal/slot system to identify and compare connections.
//

#pragma once

#include <nx/common/helpers.hpp>

#include <atomic>
#include <cstdint>
#include <cstring>
#include <type_traits>
#include <typeinfo>
#include <tuple>

namespace nx::core::detail {

using function_id_t = std::uint64_t;

// ──────────────────────────────────────────────────────────────────────────────
// Internal hashing helpers
// ──────────────────────────────────────────────────────────────────────────────

namespace {

constexpr function_id_t
fnv1a_64(const std::uint8_t * data, std::size_t n) noexcept
{
    constexpr function_id_t basis = 14695981039346656037ULL;
    constexpr function_id_t prime = 1099511628211ULL;
    function_id_t h = basis;
    for (std::size_t i = 0; i < n; ++i)
        h = (h ^ data[i]) * prime;
    return h;
}

template<typename T>
NX_NODISCARD function_id_t
bytes_fingerprint(const T & v) noexcept
{
    return fnv1a_64(reinterpret_cast<const std::uint8_t *>(&v), sizeof(v));
}

} // anonymous namespace

// ──────────────────────────────────────────────────────────────────────────────
// Public API
// ──────────────────────────────────────────────────────────────────────────────

// Free function pointer
template<typename Ret, typename... Args>
NX_NODISCARD function_id_t
get_function_id(Ret (*fp)(Args...)) noexcept
{
    return bytes_fingerprint(fp);
}

// Non-const member function pointer
template<typename Class, typename Ret, typename... Args>
NX_NODISCARD function_id_t
get_function_id(Ret (Class::*mfp)(Args...)) noexcept
{
    return bytes_fingerprint(mfp);
}

// Const member function pointer
template<typename Class, typename Ret, typename... Args>
NX_NODISCARD function_id_t
get_function_id(Ret (Class::*mfp)(Args...) const) noexcept
{
    return bytes_fingerprint(mfp);
}

// Noexcept variants
template<typename Ret, typename... Args>
NX_NODISCARD function_id_t
get_function_id(Ret (*fp)(Args...) noexcept) noexcept
{
    return bytes_fingerprint(fp);
}

template<typename Class, typename Ret, typename... Args>
NX_NODISCARD function_id_t
get_function_id(Ret (Class::*mfp)(Args...) noexcept) noexcept
{
    return bytes_fingerprint(mfp);
}

template<typename Class, typename Ret, typename... Args>
NX_NODISCARD function_id_t
get_function_id(Ret (Class::*mfp)(Args...) const noexcept) noexcept
{
    return bytes_fingerprint(mfp);
}

// Callable objects / lambdas — use address for lvalues, typeid hash for rvalues
template<typename F,
    std::enable_if_t<
        !std::is_pointer_v<std::decay_t<F>> &&
        !std::is_member_pointer_v<std::decay_t<F>>,
    int> = 0>
NX_NODISCARD function_id_t
get_function_id(F && f) noexcept
{
    if constexpr (std::is_lvalue_reference_v<F &&>) {
        return bytes_fingerprint(std::addressof(f));
    } else {
        const char * name = typeid(std::decay_t<F>).name();
        return fnv1a_64(reinterpret_cast<const std::uint8_t *>(name), std::strlen(name));
    }
}

// ──────────────────────────────────────────────────────────────────────────────
// Sender key — unique identifier for a (sender, signal) pair
// ──────────────────────────────────────────────────────────────────────────────

using sender_key_t    = std::uint64_t;
using connection_id_t = std::uint64_t;

NX_NODISCARD inline sender_key_t
make_sender_key(const void * sender, function_id_t signal_id) noexcept
{
    function_id_t s = bytes_fingerprint(sender);
    return s ^ (signal_id * 2654435761ULL);
}

// Returns a globally unique connection ID (monotonic counter).
// Using a counter instead of a content hash lets multiple identical
// (sender, signal, receiver, slot) connections coexist with distinct IDs.
NX_NODISCARD inline connection_id_t
make_connection_id() noexcept
{
    static std::atomic<connection_id_t> counter { 1 };
    return counter.fetch_add(1, std::memory_order_relaxed);
}

// ──────────────────────────────────────────────────────────────────────────────
// function_traits — extracts argument types from function pointers
// ──────────────────────────────────────────────────────────────────────────────

template<typename F>
struct function_traits;

template<typename Ret, typename... Args>
struct function_traits<Ret(*)(Args...)> {
    using return_type  = Ret;
    using args_tuple   = std::tuple<Args...>;
    using class_type   = void;
    static constexpr bool is_member = false;
    static constexpr bool is_const  = false;
};

template<typename Class, typename Ret, typename... Args>
struct function_traits<Ret(Class::*)(Args...)> {
    using return_type  = Ret;
    using args_tuple   = std::tuple<Args...>;
    using class_type   = Class;
    static constexpr bool is_member = true;
    static constexpr bool is_const  = false;
};

template<typename Class, typename Ret, typename... Args>
struct function_traits<Ret(Class::*)(Args...) const> {
    using return_type  = Ret;
    using args_tuple   = std::tuple<Args...>;
    using class_type   = Class;
    static constexpr bool is_member = true;
    static constexpr bool is_const  = true;
};

template<typename Class, typename Ret, typename... Args>
struct function_traits<Ret(Class::*)(Args...) noexcept> {
    using return_type  = Ret;
    using args_tuple   = std::tuple<Args...>;
    using class_type   = Class;
    static constexpr bool is_member = true;
    static constexpr bool is_const  = false;
};

template<typename Class, typename Ret, typename... Args>
struct function_traits<Ret(Class::*)(Args...) const noexcept> {
    using return_type  = Ret;
    using args_tuple   = std::tuple<Args...>;
    using class_type   = Class;
    static constexpr bool is_member = true;
    static constexpr bool is_const  = true;
};

// Helper alias
template<typename F>
using function_args_t = typename function_traits<std::decay_t<F>>::args_tuple;

} // namespace nx::core::detail
