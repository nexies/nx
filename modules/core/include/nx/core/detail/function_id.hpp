//
// Created by nexie on 19.02.2026.
//

#ifndef FUNCTION_ID_HPP
#define FUNCTION_ID_HPP

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <type_traits>
#include <typeinfo>

namespace nx::detail {
    using function_id = uint64_t;

    // --- 64-bit FNV-1a over bytes ---
    inline std::uint64_t fnv1a64(const std::byte *data, std::size_t n) noexcept {
        std::uint64_t h = 1469598103934665603ull;
        for (std::size_t i = 0; i < n; ++i) {
            h ^= static_cast<std::uint8_t>(data[i]);
            h *= 1099511628211ull;
        }
        return h;
    }

    inline std::uint64_t mix64(std::uint64_t h, std::uint64_t x) noexcept {
        // простое смешивание (достаточно для ID внутри процесса)
        h ^= x + 0x9e3779b97f4a7c15ull + (h << 6) + (h >> 2);
        return h;
    }

    // Уникальный "якорь" на тип: адрес inline-переменной уникален в рамках процесса.
    template<class T>
    inline const char type_anchor = 0;

    template<class T>
    std::uint64_t type_fingerprint() noexcept {
        // typeid(T).hash_code() не стабилен между запусками — и это нормально по твоим условиям.
        auto h = static_cast<std::uint64_t>(typeid(T).hash_code());

        // Добавляем адрес уникального якоря типа (обычно даёт хорошую уникальность внутри процесса).
        auto p = reinterpret_cast<std::uintptr_t>(&type_anchor<T>);
        h = mix64(h, static_cast<std::uint64_t>(p));
        return h;
    }

    template<class T>
    std::uint64_t bytes_fingerprint(const T &v) noexcept {
        static_assert(std::is_trivially_copyable_v<T>,
                      "bytes_fingerprint requires trivially copyable type");
        std::byte buf[sizeof(T)];
        std::memcpy(buf, &v, sizeof(T));
        return fnv1a64(buf, sizeof(T));
    }

    // --- The one function you asked for ---
    template<class F>
    function_id get_function_id(F &&f) noexcept {
        using D = std::remove_reference_t<F>;

        // 1) Ссылка на функцию: F — function type (не указатель)
        if constexpr (std::is_function_v<D>) {
            using FP = D *; // превращаем ссылку на функцию в указатель
            FP p = &f;
            std::uint64_t h = type_fingerprint<FP>();
            h = mix64(h, bytes_fingerprint(p));
            return h;
        }
        // 2) Указатель на функцию
        else if constexpr (std::is_pointer_v<D> &&
                           std::is_function_v<std::remove_pointer_t<D> >) {
            std::uint64_t h = type_fingerprint<D>();
            h = mix64(h, bytes_fingerprint(f));
            return h;
        }
        // 3) Указатель на метод (pointer-to-member function)
        else if constexpr (std::is_member_function_pointer_v<D>) {
            std::uint64_t h = type_fingerprint<D>();
            h = mix64(h, bytes_fingerprint(f)); // memcpy по байтам — корректно
            return h;
        }
        // 4) Лямбда / функтор (callable object)
        else {
            using T = std::remove_cv_t<D>;

            std::uint64_t h = type_fingerprint<T>();

            // Если передали lvalue-экземпляр (именованную лямбду/функтор), добавим адрес экземпляра:
            if constexpr (std::is_lvalue_reference_v<F>) {
                auto p = reinterpret_cast<std::uintptr_t>(std::addressof(f));
                h = mix64(h, static_cast<std::uint64_t>(p));
            }
            // Если это rvalue (временная лямбда `[]{} `), то уникальность будет по типу (месту в коде).

            return h;
        }
    }
}

#endif //FUNCTION_ID_HPP
