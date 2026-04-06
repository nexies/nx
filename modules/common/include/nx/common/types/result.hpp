//
// Created by nexie on 4/6/2026.
//

#ifndef NX_COMMON_RESULT_HPP
#define NX_COMMON_RESULT_HPP
#include "nx/common/helpers.hpp"

namespace nx
{
    template<typename Type, typename Error>
    class basic_result
    {

    public:
        using error_type = Error;
        using value_type = Type;

        using reference = Type &;
        using const_reference = const Type &;

    private:
        union
        {
            error_type error;
            value_type value;
        } data_;

    public:

        NX_NODISCARD const_reference
        value() const;

        NX_NODISCARD constexpr bool
        has_value () const;

        NX_NODISCARD const error_type &
        error () const;

        NX_NODISCARD constexpr bool
        is_error () const;

        NX_NODISCARD constexpr
        operator bool () const;

    public:
        explicit
        basic_result(value_type && value);
        explicit
        basic_result(error_type && error);

        explicit
        basic_result(const value_type & value);
        explicit
        basic_result(const error_type & error);

        basic_result();

        basic_result(const basic_result & other) = default;
        basic_result(basic_result && other) = default;
        basic_result & operator=(const basic_result & other) = default;
        basic_result & operator=(basic_result && other) = default;

    };

}

#endif //NX_COMMON_RESULT_HPP