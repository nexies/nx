//
// Created by nexie on 19.04.2026.
//

#ifndef NX_STATIC_STRING_HPP
#define NX_STATIC_STRING_HPP

#include <array>
#include <cstddef>
#include <string_view>
#include <utility>

namespace nx {

    template <std::size_t N>
    struct static_string {
        std::array<char, N> data{};

        constexpr static_string(const char (&str)[N]) {
            for (std::size_t i = 0; i < N; ++i) {
                data[i] = str[i];
            }
        }

        [[nodiscard]] constexpr std::size_t size() const noexcept {
            // без завершающего '\0'
            return N - 1;
        }

        [[nodiscard]] constexpr const char* c_str() const noexcept {
            return data.data();
        }

        [[nodiscard]] constexpr const char* begin() const noexcept {
            return data.data();
        }

        [[nodiscard]] constexpr const char* end() const noexcept {
            return data.data() + size();
        }

        [[nodiscard]] constexpr char operator[](std::size_t i) const noexcept {
            return data[i];
        }

        [[nodiscard]] constexpr operator std::string_view() const noexcept {
            return std::string_view(data.data(), size());
        }

        [[nodiscard]] friend constexpr bool operator==(const static_string&,
                                                       const static_string&) noexcept = default;
    };

    // deduction guide
    template <std::size_t N>
    static_string(const char (&)[N]) -> static_string<N>;
}

#endif //NX_STATIC_STRING_HPP
