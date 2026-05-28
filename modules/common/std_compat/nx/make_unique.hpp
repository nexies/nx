#pragma once

#include <memory>
#include <utility>
#include <nx/common/platform/standard_defs.hpp>

namespace nx {

#if defined(NX_CPP14) || defined(NX_CPP17) || defined(NX_CPP20) || defined(NX_CPP23)
    using std::make_unique;
#else
    // C++11: std::make_unique was added in C++14.
    // Exception safety: if T's constructor throws, operator delete is called
    // automatically by the runtime before the exception propagates — no leak.
    // The raw pointer never escapes the `new` expression before unique_ptr
    // takes ownership, so there is no window for a resource leak.
    template<typename T, typename... Args>
    std::unique_ptr<T> make_unique(Args &&... args) {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }
#endif

} // namespace nx
