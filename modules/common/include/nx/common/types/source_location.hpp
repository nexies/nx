//
// Created by nexie on 29.03.2026.
//

#ifndef NX_COMMON_SOURCE_LOCATION_HPP
#define NX_COMMON_SOURCE_LOCATION_HPP

#include <cstdint>
#include <string>
#include <nx/common/platform/compiler_defs.hpp>

#include "nx/common/helpers.hpp"

#if defined(NX_CXX_GCC)
#   define NX_BUILTIN_CURRENT_FILE          __builtin_FILE()
#   define NX_BUILTIN_CURRENT_FUNCTION      __builtin_FUNCTION()
#   define NX_BUILTIN_CURRENT_LINE          __builtin_LINE()
// #   define NX_BUILTIN_CURRENT_COLUMN        __builtin_COLUMN()

typedef uint_least16_t uint_least16;
#endif

namespace nx {
    class source_location {
        uint_least16 line_;
        const char * file_;
        const char * function_;

    public:
        explicit
        source_location(
            uint_least16 line = NX_BUILTIN_CURRENT_LINE
            , const char * file = NX_BUILTIN_CURRENT_FILE
            , const char * function = NX_BUILTIN_CURRENT_FUNCTION
            )
            : line_ { line }
            , file_ { file }
            , function_ { function } {
        }

        NX_NODISCARD uint_least16
        line () const { return line_; }

        NX_NODISCARD std::string_view
        function () const { return function_; }

        NX_NODISCARD std::string_view
        file () const { return file_; }

    };
}

#endif //NX_COMMON_SOURCE_LOCATION_HPP
