//
// Created by nexie on 29.03.2026.
//

#ifndef NX_COMMON_SOURCE_LOCATION_HPP
#define NX_COMMON_SOURCE_LOCATION_HPP

#include <cstdint>
#include <string>
#include <sstream>
#include <filesystem>
#include <nx/common/platform/compiler_defs.hpp>
#include <nx/common/helpers.hpp>

#if defined(NX_CXX_MSVC) && NX_CXX_MSVC_VER >= 1926
#   define NX_BUILTIN_FILE          __builtin_FILE()
#   define NX_BUILTIN_FUNCTION      __builtin_FUNCTION()
#   define NX_BUILTIN_LINE          __builtin_LINE()
#   define NX_BUILTIN_COLUMN        __builtin_COLUMN()
#   define NX_FUNCTION_SIGNATURE    __FUNCSIG__
#
#   define NX_HAS_BUILTIN_COLUMN    true
#elif defined(NX_CXX_CLANG) || defined(NX_CXX_GCC)
#   define NX_BUILTIN_FILE          __builtin_FILE()
#   define NX_BUILTIN_FUNCTION      __builtin_FUNCTION()
#   define NX_BUILTIN_LINE          __builtin_LINE()
#   define NX_FUNCTION_SIGNATURE    __PRETTY_FUNCTION__
#   if NX_HAS_BUILTIN(__builtin_COLUMN)
#       define NX_BUILTIN_COLUMN       __builtin_COLUMN()
#       define NX_HAS_BUILTIN_COLUMN    true
#   else
#       define NX_BUILTIN_COLUMN        0
#       define NX_HAS_BUILTIN_COLUMN    false
#   endif
#else
#   define NX_BUILTIN_FILE          __FILE__
#   define NX_BUILTIN_FUNCTION      __func__
#   define NX_BUILTIN_LINE          __LINE__
#   define NX_BUILTIN_COLUMN        0
#   define NX_FUNCTION_SIGNATURE    __func__
#   define NX_HAS_BUILTIN_COLUMN    false
#endif

using uint_least16 = std::uint_least16_t;

namespace nx {
    class source_location
    {
        uint_least16 line_;
        uint_least16 column_;
        const char * file_;
        const char * function_;

    public:
        static constexpr bool has_builtin_column = NX_HAS_BUILTIN_COLUMN;

    public:
        NX_FORCE_INLINE
        explicit constexpr
        source_location(
            uint_least16 line = NX_BUILTIN_LINE
            , uint_least16 column = NX_BUILTIN_COLUMN
            , const char * file = NX_BUILTIN_FILE
            , const char * function = NX_BUILTIN_FUNCTION
            )
        : line_ { line }
        , column_ { column }
        , file_ { file }
        , function_ { function } {
        }

        NX_NODISCARD constexpr uint_least16
        line () const { return line_; }

        NX_NODISCARD constexpr uint_least16
        column () const
        {
            if constexpr (has_builtin_column)
            {
                return column_;
            }
            return 0;
        }

        NX_NODISCARD constexpr std::string_view
        function () const { return function_; }

        NX_NODISCARD constexpr std::string_view
        file () const { return file_; }

        NX_NODISCARD std::filesystem::path
        filepath () const { return { file_ }; }

        NX_NODISCARD static constexpr source_location
        current ( source_location src = source_location() )
        { return src; }

        NX_NODISCARD std::string
        short_link () const
        {
            std::stringstream ss;
            ss << filepath().filename().string() << ":" << line_;

            if constexpr (has_builtin_column)
            {
                ss << ":" << column_;
            }

            return ss.str();
        }

        NX_NODISCARD std::string
        description () const
        {
            std::stringstream ss;
            ss << "Source Location: " << short_link() << std::endl;
            if (!function().empty())
            {
                ss << "\tFunction: " << function() << std::endl;
            }
            ss << "\tFile: " << file() << std::endl;
            ss << "\tLine: " << line() << std::endl;

            if constexpr (has_builtin_column)
                ss << "\tColumn: " << column() << std::endl;

            return ss.str();
        }
    };

    inline
    std::ostream & operator << (std::ostream & os, const source_location & src)
    {
        return os << src.description();
    }
}

#endif //NX_COMMON_SOURCE_LOCATION_HPP
