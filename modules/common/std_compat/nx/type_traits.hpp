#pragma once

#include <nx/common/platform/standard_defs.hpp>
#include <type_traits>

// ── nx type-trait helpers ─────────────────────────────────────────────────────
// Provides invoke_result_t for C++11/14 (std::result_of based) while
// transparently aliasing to std::invoke_result_t on C++17+.

namespace nx {

#if defined(NX_CPP17) || defined(NX_CPP20) || defined(NX_CPP23)

template<typename F, typename... Args>
using invoke_result_t = std::invoke_result_t<F, Args...>;

#else

// std::result_of<F(Args...)>::type is the C++11/14 equivalent.
template<typename F, typename... Args>
using invoke_result_t = typename std::result_of<F(Args...)>::type;


#endif

template<typename T>
using remove_cv_t = typename std::remove_cv<T>::type;

template<bool C, typename T = void>
using enable_if_t = typename std::enable_if<C, T>::type;

template<typename T>
using remove_reference_t = typename std::remove_reference<T>::type;

} // namespace nx
