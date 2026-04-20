//
// Created by nexie on 4/1/2026.
//

#ifndef NX_COMMON_ERROR_HPP
#define NX_COMMON_ERROR_HPP

#include <nx/common/types/errors/error_descriptor.hpp>
#include <nx/common/types/errors/named_category.hpp>
#include <system_error>

namespace nx {

    // Default category for nx::error base class.
    const std::error_category& error_category() noexcept;

    class error : public std::exception
    {
        enum flag : uint8_t { by_cat = 0, by_desc = 1 };

        int  code_;
        flag flag_;
        union {
            std::error_category*  cat;
            nx::error_descriptor* desc;
        } d_;

        NX_NODISCARD error
        get_copy() const;

        error_descriptor*
        try_alloc_desc() noexcept;

        void
        try_free_desc() noexcept;

        void
        promote_desc() noexcept;

    protected:
        // For subclasses: construct with a custom category, code, and message.
        error(const std::error_category& cat, int code, std::string_view message,
              const nx::source_location& loc = nx::source_location::current()) noexcept;

    public:
        // Neutral (no error, operator bool() == false)
        error() noexcept;

        // Generic nx error from message only (code = 1)
        explicit error(std::string_view message,
                      const nx::source_location& loc = nx::source_location::current()) noexcept;

        // Generic nx error with explicit code
        error(int code, std::string_view message,
              const nx::source_location& loc = nx::source_location::current()) noexcept;

        // std::error_code compatibility — lightweight, no descriptor
        explicit error(const std::error_code& code) noexcept;

        // std::error_code compatibility — with context comment
        error(const std::error_code& code, std::string_view comment,
              const nx::source_location& loc = nx::source_location::current()) noexcept;

        error(const error& other);
        error(error&& other) noexcept;

        error& operator=(const error& other);
        error& operator=(error&& other) noexcept;

        ~error() noexcept override;

        // Numeric error code (0 means no error)
        NX_NODISCARD int
        value() const noexcept;

        // Error category (identity distinguishes error types)
        NX_NODISCARD std::error_category const&
        category() const noexcept;

        // Standard error condition (for std::error_code interop)
        NX_NODISCARD std::error_condition
        default_error_condition() const noexcept;

        // Category-provided description (empty for nx-native errors)
        NX_NODISCARD std::string
        description() const noexcept;

        // User-defined context message
        NX_NODISCARD std::string
        comment() const noexcept;

        // Does comment() return non-empty string?
        NX_NODISCARD bool
        commented() const noexcept;

        // Formatted error string (std::exception::what())
        NX_NODISCARD const char*
        what() const noexcept override;

        // Source location of the error creation point
        NX_NODISCARD nx::source_location
        where() const noexcept;

        // Is source location captured?
        NX_NODISCARD bool
        located() const noexcept;

        // True when code != 0
        NX_NODISCARD explicit
        operator bool() const noexcept;

        // Copy with same code/category but new comment and location
        NX_NODISCARD error
        operator()(std::string_view comment,
                  const nx::source_location& loc = nx::source_location::current()) const noexcept;

        // Same code and category (ignores comment/location)
        NX_NODISCARD bool
        equivalent(const error& other) const noexcept;

        // Same code, category, comment, and location
        NX_NODISCARD bool
        identical(const error& other) const noexcept;

        NX_NODISCARD bool operator==(const error& other) const noexcept;
        NX_NODISCARD bool operator!=(const error& other) const noexcept;

        void clear() noexcept;
    };

    void explain(error const& err, FILE* out = stderr) noexcept;

} // namespace nx

#endif //NX_COMMON_ERROR_HPP
