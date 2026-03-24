//
// Created by nexie on 10.02.2026.
//

#ifndef NXTBOT_ENUM_DEFS_HPP
#define NXTBOT_ENUM_DEFS_HPP

#ifndef __nx_cxx_20_enum
#error ""
#endif

#include <nx/experimental/reflect>
#include <nx/core/meta/enum_flags.hpp>
#include <nx/core/meta/enum.hpp>
#include <nx/macro/util/platform.hpp>


# define _nx_declare_enum_formatter(enum_type) \
    inline namespace detail { \
    using _nx_concat_3(enum_type, _, formatter) = ::fmt::formatter<enum_type, char>; \
    }


# define _nx_meta_enum(enum_type) \
    _nx_declare_enum_formatter(enum_type)


#define NX_ENUM(EnumType) \
    _nx_meta_enum(EnumType)

#endif //NXTBOT_ENUM_DEFS_HPP