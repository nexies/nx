//
// Created by nexie on 4/6/2026.
//

#ifndef NX_COMMON_RESULT_HPP
#define NX_COMMON_RESULT_HPP
#include "../../../../../../third-party/spdlog/include/spdlog/fmt/bundled/base.h"
#include "errors/codes.hpp"
#include "nx/common/helpers.hpp"

namespace nx
{
    template<typename Type, typename Error>
    class basic_result {
    public:
        using error_type = Error;
        using value_type = Type;

        using reference = Type &;
        using const_reference = const Type &;

        using expect_handler = std::function<value_type(const error_type &)>;

    private:
        std::variant<value_type, error_type> data_;
        bool is_error_;

        void
        make_value (value_type && val) {
            data_.template emplace<value_type>(std::forward<value_type>(val));
        }

        void
        remove_value() {
            // reinterpret_cast<value_type *>(&data_)->~value_type();
        }

        void
        make_error(error_type && err) {
            data_.template emplace<error_type>(std::forward<error_type>(err));
        }


    public:

        NX_NODISCARD constexpr const_reference
        value()  {
            if (!is_error_)
                return std::get<value_type>(data_);
            throw nx::err::invalid_argument("nx::basic_result is not a value");
        }

        NX_NODISCARD constexpr value_type
        value_or(const value_type & or_value) {
            if (!is_error_)
                return std::get<value_type>(data_);
            return or_value;
        }

        NX_NODISCARD constexpr bool
        has_value () const noexcept {
            return !is_error_;
        }

        NX_NODISCARD constexpr const error_type &
        error () const {
            if (is_error_)
                return std::get<error_type>(data_);
            throw nx::err::invalid_argument("nx::basic_result is not an error");
        }

        NX_NODISCARD constexpr bool
        is_error () const noexcept {
            return is_error_;
        }

        NX_NODISCARD value_type
        expect (const expect_handler & handler) {
            if (is_error_)
                return handler(error());
            return value();
        }

        NX_NODISCARD constexpr
        operator bool () const {
            return this->has_value();
        }

        NX_NODISCARD
        operator Type () noexcept {
            return this->value_or({});
        }

    public:
        basic_result(value_type && value)
            : is_error_ { false }
            , data_ { } {
            make_value(std::forward<value_type>(value));
        }

        basic_result(error_type && error)
            : is_error_ { true }
            , data_ {} {
            make_error(std::forward<error_type>(error));
        }

        basic_result(const value_type & value)
            : is_error_ { false }
            , data_ {} {
            make_value(value);
        }

        basic_result(const error_type & error)
            : is_error_ { true }
            , data_ {} {
            make_error(error);
        }

        basic_result()
            : is_error_ { false }
            , data_ { } {
            make_value(value_type());
        }

        basic_result(const basic_result & other) = default;
        basic_result(basic_result && other) = default;
        basic_result & operator=(const basic_result & other) = default;
        basic_result & operator=(basic_result && other) = default;
    };

    template<typename Value>
    using result = basic_result<Value, nx::error>;

}

#endif //NX_COMMON_RESULT_HPP