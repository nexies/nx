//
// Created by nexie on 4/20/2026.
//

#ifndef NX_CORE2_META_ERROR_HPP
#define NX_CORE2_META_ERROR_HPP

#include <nx/common/types/errors.hpp>

namespace nx::err
{
    class meta_error : public error
    {
        static const std::error_category& s_category() noexcept
        {
            const static named_error_category cat { "meta" };
            return cat;
        }

    protected:
        meta_error(const std::error_category& cat, int code, std::string_view msg,
                    const nx::source_location& loc) noexcept
            : error(cat, code, msg, loc) {}
    public:

        explicit
        meta_error(std::string_view msg,
            const nx::source_location& loc = nx::source_location::current()) noexcept
        : meta_error(s_category(), 1, msg, loc) {}


        meta_error(int code, std::string_view msg,
            const nx::source_location& loc = nx::source_location::current()) noexcept
        : meta_error(s_category(), code, msg, loc) {}

    };


    class property_error : public meta_error
    {
        static const std::error_category& s_category() noexcept
        {
            const static named_error_category cat { "property" };
            return cat;
        }
    public:
        explicit
        property_error(std::string_view msg,
            const nx::source_location& loc = nx::source_location::current()) noexcept
        : meta_error(msg, loc) {}


        property_error(int code, std::string_view msg,
            const nx::source_location& loc = nx::source_location::current()) noexcept
        : meta_error(s_category(), code, msg, loc) {}
    };
}

#endif //NX_CORE2_META_ERROR_HPP
