//
// Created by nexie on 4/6/2026.
//

#ifndef NX_COMMON_ERROR_CODES_HPP
#define NX_COMMON_ERROR_CODES_HPP

#include <nx/common/types/errors/error.hpp>

namespace nx::err {

    // ── nx::runtime_error ────────────────────────────────────────────────────
    // Errors caused by unforeseen runtime conditions (I/O failure, OS errors, etc.)

    class runtime_error : public error {
        static const std::error_category& s_category() noexcept {
            static const named_error_category cat { "runtime" };
            return cat;
        }
    protected:
        runtime_error(const std::error_category& cat, int code, std::string_view msg,
                      const nx::source_location& loc) noexcept
            : error(cat, code, msg, loc) {}
    public:
        explicit runtime_error(std::string_view msg,
                               const nx::source_location& loc = nx::source_location::current()) noexcept
            : error(s_category(), 1, msg, loc) {}

        runtime_error(int code, std::string_view msg,
                      const nx::source_location& loc = nx::source_location::current()) noexcept
            : error(s_category(), code, msg, loc) {}
    };

    // ── nx::logic_error ──────────────────────────────────────────────────────
    // Errors caused by incorrect usage of an API or violated preconditions

    class logic_error : public error {
        static const std::error_category& s_category() noexcept {
            static const named_error_category cat { "logic" };
            return cat;
        }
    protected:
        logic_error(const std::error_category& cat, int code, std::string_view msg,
                    const nx::source_location& loc) noexcept
            : error(cat, code, msg, loc) {}
    public:
        explicit logic_error(std::string_view msg,
                             const nx::source_location& loc = nx::source_location::current()) noexcept
            : error(s_category(), 1, msg, loc) {}

        logic_error(int code, std::string_view msg,
                    const nx::source_location& loc = nx::source_location::current()) noexcept
            : error(s_category(), code, msg, loc) {}
    };

    // ── nx::memory_error ─────────────────────────────────────────────────────
    // Memory allocation or access errors

    class memory_error : public error {
        static const std::error_category& s_category() noexcept {
            static const named_error_category cat { "memory" };
            return cat;
        }
    protected:
        memory_error(const std::error_category& cat, int code, std::string_view msg,
                     const nx::source_location& loc) noexcept
            : error(cat, code, msg, loc) {}
    public:
        explicit memory_error(std::string_view msg,
                              const nx::source_location& loc = nx::source_location::current()) noexcept
            : error(s_category(), 1, msg, loc) {}

        memory_error(int code, std::string_view msg,
                     const nx::source_location& loc = nx::source_location::current()) noexcept
            : error(s_category(), code, msg, loc) {}
    };

    // ── nx::access_error : runtime_error ─────────────────────────────────────
    // Permission or access control failures

    class access_error : public runtime_error {
        static const std::error_category& s_category() noexcept {
            static const named_error_category cat { "access" };
            return cat;
        }
    public:
        explicit access_error(std::string_view msg,
                              const nx::source_location& loc = nx::source_location::current()) noexcept
            : runtime_error(s_category(), 1, msg, loc) {}

        access_error(int code, std::string_view msg,
                     const nx::source_location& loc = nx::source_location::current()) noexcept
            : runtime_error(s_category(), code, msg, loc) {}
    };

    // ── nx::invalid_argument : logic_error ───────────────────────────────────
    // A function argument has an invalid value or type

    class invalid_argument : public logic_error {
        static const std::error_category& s_category() noexcept {
            static const named_error_category cat { "invalid_argument" };
            return cat;
        }
    public:
        explicit invalid_argument(std::string_view msg,
                                  const nx::source_location& loc = nx::source_location::current()) noexcept
            : logic_error(s_category(), 1, msg, loc) {}

        invalid_argument(int code, std::string_view msg,
                         const nx::source_location& loc = nx::source_location::current()) noexcept
            : logic_error(s_category(), code, msg, loc) {}
    };

    // ── nx::not_supported : logic_error ──────────────────────────────────────
    // Feature or operation is not implemented or not available

    class not_supported : public logic_error {
        static const std::error_category& s_category() noexcept {
            static const named_error_category cat { "not_supported" };
            return cat;
        }
    public:
        explicit not_supported(std::string_view msg,
                               const nx::source_location& loc = nx::source_location::current()) noexcept
            : logic_error(s_category(), 1, msg, loc) {}

        not_supported(int code, std::string_view msg,
                      const nx::source_location& loc = nx::source_location::current()) noexcept
            : logic_error(s_category(), code, msg, loc) {}
    };

    // ── nx::invalid_state : logic_error ──────────────────────────────────────
    // Object is in a state that does not allow the requested operation

    class invalid_state : public logic_error {
        static const std::error_category& s_category() noexcept {
            static const named_error_category cat { "invalid_state" };
            return cat;
        }
    public:
        explicit invalid_state(std::string_view msg,
                               const nx::source_location& loc = nx::source_location::current()) noexcept
            : logic_error(s_category(), 1, msg, loc) {}

        invalid_state(int code, std::string_view msg,
                      const nx::source_location& loc = nx::source_location::current()) noexcept
            : logic_error(s_category(), code, msg, loc) {}
    };

    class bad_cast : public runtime_error
    {
        static const std::error_category& s_category() noexcept
        {
            static const named_error_category cat { "bad_cast" };
            return cat;
        }
    public:
        explicit bad_cast(std::string_view msg,
                          const nx::source_location & loc = nx::source_location::current()) noexcept
            : runtime_error(s_category(), 2, msg, loc) {}
    };

} // namespace nx

#endif //NX_COMMON_ERROR_CODES_HPP
