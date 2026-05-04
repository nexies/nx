//
// Created by nexie on 4/6/2026.
//

#ifndef NX_COMMON_RESULT_HPP
#define NX_COMMON_RESULT_HPP

#include <nx/common/types/errors/codes.hpp>
#include <nx/common/helpers.hpp>

#include <functional>
#include <optional>
#include <variant>

namespace nx
{
    template<typename Type, typename Error>
    class basic_result {
    public:
        using error_type       = Error;
        using value_type       = Type;
        using reference        = Type &;
        using const_reference  = const Type &;
        using pointer          = Type *;
        using const_pointer    = const Type *;

        using expect_handler = std::function<value_type(const error_type &)>;

    private:
        std::variant<value_type, error_type> data_;
        bool is_error_;

    public:
        // ── Value access ──────────────────────────────────────────────────────

        NX_NODISCARD constexpr const_reference
        value() const {
            if (!is_error_)
                return std::get<value_type>(data_);
            throw nx::err::invalid_state("nx::basic_result is not a value");
        }

        NX_NODISCARD constexpr reference
        value() {
            if (!is_error_)
                return std::get<value_type>(data_);
            throw nx::err::invalid_state("nx::basic_result is not a value");
        }

        NX_NODISCARD constexpr const_reference
        operator*() const { return value(); }

        NX_NODISCARD constexpr reference
        operator*() { return value(); }

        NX_NODISCARD constexpr const_pointer
        operator->() const { return &value(); }

        NX_NODISCARD constexpr pointer
        operator->() { return &value(); }

        NX_NODISCARD constexpr value_type
        value_or(const value_type & or_value) const {
            if (!is_error_)
                return std::get<value_type>(data_);
            return or_value;
        }

        // ── Error access ──────────────────────────────────────────────────────

        NX_NODISCARD constexpr const error_type &
        error() const {
            if (is_error_)
                return std::get<error_type>(data_);
            throw nx::err::invalid_state("nx::basic_result is not an error");
        }

        // ── State ─────────────────────────────────────────────────────────────

        NX_NODISCARD constexpr bool
        has_value() const noexcept { return !is_error_; }

        NX_NODISCARD constexpr bool
        is_error() const noexcept { return is_error_; }

        NX_NODISCARD constexpr explicit
        operator bool() const noexcept { return has_value(); }

        // ── Monadic helpers ───────────────────────────────────────────────────

        NX_NODISCARD value_type
        expect(expect_handler && handler) {
            if (is_error_)
                return handler(error());
            return value();
        }

        // ── Constructors ──────────────────────────────────────────────────────

        // Constructors use std::in_place_type to avoid requiring a default
        // constructor on value_type.

        basic_result(value_type && v)
            : is_error_ { false }
            , data_ { std::in_place_type<value_type>, std::forward<value_type>(v) } {}

        basic_result(const value_type & v)
            : is_error_ { false }
            , data_ { std::in_place_type<value_type>, v } {}

        basic_result(error_type && e)
            : is_error_ { true }
            , data_ { std::in_place_type<error_type>, std::forward<error_type>(e) } {}

        basic_result(const error_type & e)
            : is_error_ { true }
            , data_ { std::in_place_type<error_type>, e } {}

        // Only available when value_type is default-constructible.
        template<typename U = value_type,
                 std::enable_if_t<std::is_default_constructible_v<U>, int> = 0>
        basic_result()
            : is_error_ { false }
            , data_ { std::in_place_type<value_type> } {}

        ~basic_result() = default;

        basic_result(const basic_result &)            = default;
        basic_result(basic_result &&)                 = default;
        basic_result & operator=(const basic_result &) = default;
        basic_result & operator=(basic_result &&)      = default;
    };

    // ──────────────────────────────────────────────────────────────────────────
    // basic_result<void, Error> — specialization for operations that return no
    // value on success (only succeed or fail with an error).
    // ──────────────────────────────────────────────────────────────────────────

    template<typename Error>
    class basic_result<void, Error> {
    public:
        using error_type = Error;
        using value_type = void;

    private:
        std::optional<Error> error_;

    public:
        // ── State ─────────────────────────────────────────────────────────────

        NX_NODISCARD constexpr bool
        has_value() const noexcept { return !error_.has_value(); }

        NX_NODISCARD constexpr bool
        is_error() const noexcept { return error_.has_value(); }

        NX_NODISCARD constexpr explicit
        operator bool() const noexcept { return has_value(); }

        // ── Value / error access ──────────────────────────────────────────────

        // Throws if this is an error; useful in generic code.
        constexpr void
        value() const {
            if (is_error())
                throw nx::err::invalid_state("nx::basic_result<void> is not a value");
        }

        NX_NODISCARD constexpr const error_type &
        error() const {
            if (is_error())
                return *error_;
            throw nx::err::invalid_state("nx::basic_result<void> is not an error");
        }

        // ── Monadic helpers ───────────────────────────────────────────────────

        template<typename Handler>
        void expect(Handler && handler) const {
            if (is_error())
                std::forward<Handler>(handler)(error());
        }

        // ── Constructors ──────────────────────────────────────────────────────

        basic_result() noexcept : error_(std::nullopt) {}

        basic_result(Error && e)       : error_(std::move(e)) {}
        basic_result(const Error & e)  : error_(e) {}

        basic_result(const basic_result &)             = default;
        basic_result(basic_result &&)                  = default;
        basic_result & operator=(const basic_result &) = default;
        basic_result & operator=(basic_result &&)      = default;
        ~basic_result()                                = default;
    };

    template<typename Value>
    using result = basic_result<Value, nx::error>;

}

#endif //NX_COMMON_RESULT_HPP