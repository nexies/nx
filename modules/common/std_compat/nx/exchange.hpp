#pragma once

#include <utility>
#include <nx/common/platform/standard_defs.hpp>

namespace nx {

#if defined(NX_CPP14) || defined(NX_CPP17) || defined(NX_CPP20) || defined(NX_CPP23)
    using std::exchange;
#else
    template<typename T, typename U = T>
    T exchange(T & obj, U && new_val) {
        T old = std::move(obj);
        obj = std::forward<U>(new_val);
        return old;
    }
#endif

} // namespace nx
