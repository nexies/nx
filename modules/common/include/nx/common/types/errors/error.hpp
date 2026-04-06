//
// Created by nexie on 4/1/2026.
//

#ifndef NX_COMMON_ERROR_HPP
#define NX_COMMON_ERROR_HPP

#include <nx/common/types/errors/error_descriptor.hpp>
#include <system_error>

namespace nx
{
    class error : public std::exception
    {
        enum flag : uint8_t
        {
            by_cat = 0,
            by_desc = 1
        };

        int code_;
        flag flag_;
        union
        {
            std::error_category * cat;
            nx::error_descriptor * desc;
        }d_;

        NX_NODISCARD error
        get_copy () const;

        // void
        // set_descriptor (const error_descriptor & desc);

        error_descriptor *
        try_alloc_desc () noexcept;

        void
        try_free_desc () noexcept;

        void
        promote_desc() noexcept;

    public:
        error() noexcept;

        explicit
        error (std::errc errc) noexcept;

        error (int code, std::error_category * cat);

        explicit
        error (const std::error_code & code) noexcept;

        error (int code, std::error_category * cat, std::string_view message, const nx::source_location & loc = nx::source_location::current()) noexcept;

        error (const std::error_code & code, std::string_view comment, const nx::source_location & loc = nx::source_location::current()) noexcept;

        error (const error & other);
        error (error && other) noexcept;

        error & operator=(const error & other);
        error & operator=(error && other) noexcept;

        ~error () noexcept override;

        /// Error value
        /// @return
        NX_NODISCARD int
        value () const noexcept;

        /// Error category
        /// @return
        NX_NODISCARD std::error_category const &
        category () const noexcept;

        /// Error condition (unified error code)
        /// @return
        NX_NODISCARD std::error_condition
        default_error_condition () const noexcept;

        /// Default error value description
        NX_NODISCARD std::string
        description () const noexcept;

        /// User-defined unique message value
        NX_NODISCARD std::string
        comment () const noexcept;

        /// Does comment() return non-empty string?
        NX_NODISCARD bool
        commented () const noexcept;

        /// Short description of the error
        /// (used for explaining the error when in `catch` expressions)
        NX_NODISCARD const char*
        what () const noexcept override;

        /// Source location of the creation point
        NX_NODISCARD nx::source_location
        where () const noexcept;

        /// Is source location specified ?
        NX_NODISCARD bool
        located () const noexcept;

        /// operator bool
        explicit
        operator bool () const noexcept;

        /// Create another error object, with the same code_,
        /// but containing a comment `comment`
        NX_NODISCARD error
        operator () (std::string_view comment, const nx::source_location & loc = nx::source_location::current()) const noexcept;

        void
        clear () noexcept;
    };

    void explain (error const & err) noexcept;
}

#endif //NX_COMMON_ERROR_HPP