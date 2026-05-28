//
// Created by nexie on 19.04.2026.
//

#ifndef NX_STATIC_STRING_HPP
#define NX_STATIC_STRING_HPP

#include <array>
#include <cstddef>
#include <nx/string_view.hpp>
#include <nx/common/helpers.hpp>

namespace nx {

    template <std::size_t N>
    struct static_string {
        std::array<char, N> data{};

        constexpr static_string(const char (&str)[N]) {
            for (std::size_t i = 0; i < N; ++i) {
                data[i] = str[i];
            }
        }

        NX_NODISCARD constexpr std::size_t size() const noexcept {
            return N - 1;
        }

        NX_NODISCARD constexpr const char* c_str() const noexcept {
            return data.data();
        }

        NX_NODISCARD constexpr const char* begin() const noexcept {
            return data.data();
        }

        NX_NODISCARD constexpr const char* end() const noexcept {
            return data.data() + size();
        }

        NX_NODISCARD constexpr char operator[](std::size_t i) const noexcept {
            return data[i];
        }

        NX_NODISCARD constexpr operator nx::string_view() const noexcept {
            return nx::string_view(data.data(), size());
        }

        // Explicit comparison — C++20 `= default` not available in C++11/14
        NX_NODISCARD friend constexpr bool operator==(const static_string& a,
                                                       const static_string& b) noexcept {
            for (std::size_t i = 0; i < N; ++i)
                if (a.data[i] != b.data[i]) return false;
            return true;
        }
        NX_NODISCARD friend constexpr bool operator!=(const static_string& a,
                                                       const static_string& b) noexcept {
            return !(a == b);
        }
    };

#if defined(NX_CPP17) || defined(NX_CPP20) || defined(NX_CPP23)
    // Deduction guide — C++17 feature
    template <std::size_t N>
    static_string(const char (&)[N]) -> static_string<N>;
#endif

}

#endif //NX_STATIC_STRING_HPP
