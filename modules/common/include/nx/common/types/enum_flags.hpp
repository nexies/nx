//
// enum_flags — type-safe bitfield wrapper for enum classes.
//
// Usage:
//   enum class my_flag : uint8_t { read = 0x01, write = 0x02, exec = 0x04 };
//   NX_FLAGS(my_flag, my_flags)
//
//   my_flags f = my_flag::read | my_flag::write;
//   if (f.has(my_flag::read)) { ... }
//

#pragma once

#include <nx/common/helpers.hpp>
#include <limits>
#include <type_traits>

namespace nx::detail
{
    template <class Enum, std::underlying_type<Enum>::type AllMask = std::numeric_limits<typename std::underlying_type<Enum>::type>::max()>
    class make_flags_from_enum {
        static_assert(std::is_enum_v<Enum>, "make_flags_from_enum<T>: T must be an enum type.");

    public:
        using enum_type = Enum;
        using underlying_type = std::underlying_type_t<Enum>;

        // Конструкторы
        constexpr make_flags_from_enum() noexcept : bits_(0) {}
        constexpr make_flags_from_enum(Enum e) noexcept : bits_(to_u(e) & mask_u()) {}
        constexpr make_flags_from_enum (underlying_type u) noexcept : bits_(u) { }

        // Доступ/преобразования
        constexpr underlying_type raw() const noexcept { return bits_; }
        constexpr explicit operator bool() const noexcept { return bits_ != 0; }

        // Базовые утилиты
        constexpr void clear() noexcept { bits_ = 0; }

        constexpr bool has(Enum e) const noexcept { // "содержит все биты e"
            const auto v = to_u(e) & mask_u();
            return (bits_ & v) == v;
        }

        constexpr bool contains_all(make_flags_from_enum other) const noexcept {
            return (bits_ & other.bits_) == other.bits_;
        }
        constexpr bool contains_all(Enum e) const noexcept {
            return contains_all(make_flags_from_enum(e));
        }

        constexpr bool contains_any(make_flags_from_enum other) const noexcept {
            return (bits_ & other.bits_) != 0;
        }
        constexpr bool contains_any(Enum e) const noexcept {
            return contains_any(make_flags_from_enum(e));
        }

        constexpr void set(Enum e) noexcept { bits_ |= (to_u(e) & mask_u()); }
        constexpr void unset(Enum e) noexcept { bits_ &= ~(to_u(e) & mask_u()); }
        constexpr void reset(Enum e) noexcept { bits_ &= ~(to_u(e) & mask_u()); }

        // Присваивающие операторы с Flags
        constexpr make_flags_from_enum& operator|=(make_flags_from_enum rhs) noexcept {
            bits_ = (bits_ | rhs.bits_) & mask_u();
            return *this;
        }
        constexpr make_flags_from_enum& operator&=(make_flags_from_enum rhs) noexcept {
            bits_ = (bits_ & rhs.bits_) & mask_u();
            return *this;
        }
        constexpr make_flags_from_enum& operator^=(make_flags_from_enum rhs) noexcept {
            bits_ = (bits_ ^ rhs.bits_) & mask_u();
            return *this;
        }

        // Присваивающие операторы с Enum
        constexpr make_flags_from_enum& operator|=(Enum rhs) noexcept { return (*this |= make_flags_from_enum(rhs)); }
        constexpr make_flags_from_enum& operator&=(Enum rhs) noexcept { return (*this &= make_flags_from_enum(rhs)); }
        constexpr make_flags_from_enum& operator^=(Enum rhs) noexcept { return (*this ^= make_flags_from_enum(rhs)); }

        // Унарный NOT: инвертируем только допустимые биты
        friend constexpr make_flags_from_enum operator~(make_flags_from_enum a) noexcept {
            a.bits_ = (~a.bits_) & mask_u();
            return a;
        }

        // Бинарные операторы Flags <op> Flags
        friend constexpr make_flags_from_enum operator|(make_flags_from_enum a, make_flags_from_enum b) noexcept {
            a |= b; return a;
        }
        friend constexpr make_flags_from_enum operator&(make_flags_from_enum a, make_flags_from_enum b) noexcept {
            a &= b; return a;
        }
        friend constexpr make_flags_from_enum operator^(make_flags_from_enum a, make_flags_from_enum b) noexcept {
            a ^= b; return a;
        }

        // Flags <op> Enum
        friend constexpr make_flags_from_enum operator|(make_flags_from_enum a, Enum b) noexcept { a |= b; return a; }
        friend constexpr make_flags_from_enum operator&(make_flags_from_enum a, Enum b) noexcept { a &= b; return a; }
        friend constexpr make_flags_from_enum operator^(make_flags_from_enum a, Enum b) noexcept { a ^= b; return a; }

        // Enum <op> Flags
        friend constexpr make_flags_from_enum operator|(Enum a, make_flags_from_enum b) noexcept { b |= a; return b; }
        friend constexpr make_flags_from_enum operator&(Enum a, make_flags_from_enum b) noexcept { b &= a; return b; }
        friend constexpr make_flags_from_enum operator^(Enum a, make_flags_from_enum b) noexcept { b ^= a; return b; }


        // Сравнения
        friend constexpr bool operator==(make_flags_from_enum a, make_flags_from_enum b) noexcept {
            return a.bits_ == b.bits_;
        }
        friend constexpr bool operator!=(make_flags_from_enum a, make_flags_from_enum b) noexcept {
            return !(a == b);
        }

        friend constexpr bool operator==(make_flags_from_enum a, Enum b) noexcept {
            return a.bits_ == (to_u(b) & mask_u());
        }
        friend constexpr bool operator!=(make_flags_from_enum a, Enum b) noexcept {
            return !(a == b);
        }

        friend constexpr bool operator==(Enum a, make_flags_from_enum b) noexcept { return b == a; }
        friend constexpr bool operator!=(Enum a, make_flags_from_enum b) noexcept { return !(b == a); }

        friend constexpr bool operator==(Enum a, Enum b) noexcept { // иногда удобно
            return (to_u(a) & mask_u()) == (to_u(b) & mask_u());
        }
        friend constexpr bool operator!=(Enum a, Enum b) noexcept { return !(a == b); }

    private:
        underlying_type bits_;

        static constexpr underlying_type to_u(Enum e) noexcept {
            return static_cast<underlying_type>(e);
        }

        static constexpr underlying_type mask_u() noexcept {
            return AllMask;
        }



    };
}

#define NX_FLAGS(Enum, Flags) \
    using Flags = ::nx::detail::make_flags_from_enum<Enum>; \
    constexpr Flags operator|(const Enum a, const Enum b) noexcept { \
        return Flags { static_cast<Flags::underlying_type>( \
            static_cast<Flags::underlying_type>(a) | static_cast<Flags::underlying_type>(b)) }; \
    } \
    constexpr Flags operator&(const Enum a, const Enum b) noexcept { \
        return Flags { static_cast<Flags::underlying_type>( \
            static_cast<Flags::underlying_type>(a) & static_cast<Flags::underlying_type>(b)) }; \
    } \
    constexpr Flags operator^(const Enum a, const Enum b) noexcept { \
        return Flags { static_cast<Flags::underlying_type>( \
            static_cast<Flags::underlying_type>(a) ^ static_cast<Flags::underlying_type>(b)) }; \
    }


///
/// constexpr MyFlags operator|(const MyFlag a, const MyFlag b) noexcept {
/// return MyFlags {static_cast<MyFlags::underlying_type>(a) | static_cast<MyFlags::underlying_type>(b) };
/// }
/// constexpr MyFlags operator&(const MyFlag a, const MyFlag b) noexcept {
/// return MyFlags {static_cast<MyFlags::underlying_type>(a) & static_cast<MyFlags::underlying_type>(b) };
/// }
/// constexpr MyFlags operator^(const MyFlag a, const MyFlag b) noexcept {
/// return MyFlags {static_cast<MyFlags::underlying_type>(a) ^ static_cast<MyFlags::underlying_type>(b) };
/// }

