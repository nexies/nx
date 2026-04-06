//
// Created by nexie on 4/1/2026.
//

#include <nx/common/types/errors/error.hpp>

namespace nx
{
    error error::get_copy() const
    {
        if (desc_)
        {
            error out { code_ };

        }
    }

    void error::set_descriptor(const error_descriptor& desc)
    {
        if (desc_)
            try_free_desc();

        desc_ = make_error_descriptor(desc);
    }

    error_descriptor* error::try_alloc_desc() noexcept
    {
        if (desc_)
            return desc_;
        desc_ = make_error_descriptor(g_undefined_location, "");
        return desc_;
    }

    void error::try_free_desc() noexcept
    {
        if (!desc_)
            return;
        free_error_descriptor(desc_);
        desc_ = nullptr;
    }

    error::error() noexcept
        : code_ {}
        , desc_ { nullptr }
    {

    }

    error::error(int code, std::error_category* cat)
        : code_ { code }
        , flag_ { by_cat }
        , d_ { cat }
    {

    }

    error::error(int code, std::error_category* cat, std::string_view comment, const nx::source_location& loc) noexcept
        : code_ { code }
        , flag_ { by_desc }
        , d_ { nullptr }
    {
        d_.desc = try_alloc_desc();
        if (!d_.desc)
        {
            flag_ = by_cat;
            d_.cat = cat;
            return;
        }

        d_.desc->category = cat;
        d_.desc->location = loc;
        d_.desc->comment = comment;
    }

    // error::error(std::errc errc) noexcept
    //     : code_ { std::make_error_code(errc).value() }
    //     , desc_ { std::make_error_code(errc).category() }
    // {
    //
    // }
    //
    // error::error(const std::error_code& code) noexcept
    //     : code_ { code }
    //     , desc_ { nullptr }
    // {
    //
    // }
    //
    // error::error(int code, const std::error_category& cat) noexcept
    //     : code_ (code, cat)
    //     , desc_ { nullptr }
    // {
    // }
    //
    // error::error(const std::error_code& code, std::string_view comment) noexcept
    //     : code_ { code }
    //     , desc_ { try_alloc_desc() }
    // {
    //     if (desc_)
    //         desc_->comment = comment;
    // }
    //
    error::error(const std::error_code& code, std::string_view message, const nx::source_location& loc) noexcept
        : code_ { code }
        , desc_ { try_alloc_desc() }
    {
        if (desc_)
        {
            desc_->comment = message;
            desc_->location = loc;
        }
    }

    error::error(const error& other)
        : code_ {}
        , desc_ { nullptr }
    {
        *this = other;
    }

    error::error(error&& other) noexcept
        : code_ {}
        , desc_ { nullptr }
    {
        *this = other;
    }

    error& error::operator=(const error& other)
    {
        clear();
        code_ = other.code_;
        if (other.desc_)
        {
            desc_ = make_error_descriptor(*other.desc_);
        }
        return *this;
    }

    error& error::operator=(error&& other) noexcept
    {
        code_ = other.code_;
        desc_ = other.desc_;

        other.desc_ = nullptr;
        other.clear();
        return *this;
    }

    error::~error() noexcept
    {
        if (desc_)
            try_free_desc();
    }

    int error::value() const noexcept
    {
        return code_.value();
    }

    std::error_category const& error::category() const noexcept
    {
        return code_.category();
    }

    std::error_condition error::default_error_condition() const noexcept
    {
        return code_.default_error_condition();
    }

    std::string error::description() const noexcept
    {
        return code_.message();
    }

    std::string error::comment() const noexcept
    {
        if (desc_)
            return desc_->comment;
        return {};
    }

    bool error::commented() const noexcept
    {
        return (desc_) && (!desc_->comment.empty());
    }

    const char* error::what() const noexcept
    {
        if (!desc_)
            return code_.message().data(); // TODO: Does not work

        if (desc_->what_cache.empty())
        {
            std::stringstream ss;
            ss << "[" << code_.category().name() << "_error] code " << code_.value() << " - " << code_.message();

            if (!desc_->comment.empty())
            {
                ss << std::endl << "Comment: " << desc_->comment;
            }

            if (desc_->location != g_undefined_location)
            {
                ss << std::endl << "Location: " << desc_->location.short_link();
                ss << std::endl << "\tFunction: " << desc_->location.function();
                ss << std::endl << "\tFile: " << desc_->location.file();
                ss << std::endl << "\tLine: " << desc_->location.line();
                if constexpr(nx::source_location::has_builtin_column)
                {
                    ss << std::endl << "\tColumn: " << desc_->location.column();
                }
            }

            desc_->what_cache = ss.str();
        }
        return desc_->what_cache.c_str();
    }

    nx::source_location error::where() const noexcept
    {
        if (desc_)
            return desc_->location;
        return g_undefined_location;
    }

    bool error::located() const noexcept
    {
        return desc_ && (desc_->location != g_undefined_location);
    }

    error::operator bool() const noexcept
    {
        return static_cast<bool>(code_);
    }

    error error::operator()(std::string_view comment) const noexcept
    {
        auto copy = get_copy();
        copy.desc_->comment = comment;
        return copy;
    }

    error error::operator()(const nx::source_location& loc, std::string_view comment) const noexcept
    {
        auto copy = get_copy();
        copy.desc_->comment = comment;
        copy.desc_->location = loc;
        return copy;
    }

    void error::clear() noexcept
    {
        try_free_desc();
        code_.clear();
    }
}
