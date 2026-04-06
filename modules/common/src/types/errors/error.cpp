//
// Created by nexie on 4/1/2026.
//

#include <iostream>
#include <nx/common/types/errors/error.hpp>

namespace nx
{
    error error::get_copy() const
    {
        if (flag_ == by_cat)
        {
            return error(code_, d_.cat);
        }
        return error(code_, d_.desc->category, d_.desc->comment, d_.desc->location);
    }

    error_descriptor* error::try_alloc_desc() noexcept
    {
        if (flag_ == by_desc)
            return d_.desc;

        auto tmp = make_error_descriptor(g_undefined_location, "");
        if (tmp)
        {
            tmp->category = d_.cat;
            d_.desc = tmp;
            flag_ = by_desc;
            return d_.desc;
        }
        return nullptr;
    }

    void error::try_free_desc() noexcept
    {
        if (flag_ == by_cat)
            return;
        auto tmp = d_.desc->category;
        free_error_descriptor(d_.desc);
        d_.cat = tmp;
        flag_ = by_cat;
    }

    void error::promote_desc() noexcept
    {
        if (flag_ == by_cat)
        {
            auto tmp_cat = d_.cat;
            d_.desc = try_alloc_desc();

            if (!d_.desc)
            {
                d_.cat = tmp_cat;
                return;
            }
            d_.desc->category = tmp_cat;
        }
    }

    error::error() noexcept
        : code_ { 0 }
        , flag_ {by_cat}
        , d_ { nullptr }
    {
        d_.cat = const_cast<std::error_category*>(&(std::system_category()));
    }

    error::error(std::errc errc) noexcept :
        error(std::make_error_code(errc))
    {

    }

    error::error(int code, std::error_category* cat)
        : code_ { code }
        , flag_ { by_cat }
        , d_ { cat }
    {

    }

    error::error(const std::error_code& code) noexcept
        : error(code.value(), const_cast<std::error_category *>(&code.category()))
    {

    }

    error::error(int code, std::error_category* cat, std::string_view comment, const nx::source_location& loc) noexcept
        : code_ { code }
        , flag_ { by_cat }
        , d_ { nullptr }
    {
        d_.desc = try_alloc_desc();
        if (!d_.desc)
        {
            flag_ = by_cat;
            d_.cat = cat;
            return;
        }

        flag_ = by_desc;
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
        : error( code.value(), const_cast<std::error_category *>(&(code.category())), message, loc )
    {

    }

    error::error(const error& other)
        : code_ {}
        , d_ { nullptr }
    {
        code_ = other.code_;
        if (other.flag_ == by_desc)
        {
            d_.desc = make_error_descriptor(*other.d_.desc);
        }
        else
        {
            d_.cat = other.d_.cat;
        }
    }

    error::error(error&& other) noexcept
        : code_ {}
        , d_ { nullptr }
    {
        code_ = other.code_;
        d_ = other.d_;
        flag_ = other.flag_;

        other.flag_ = by_cat;
        other.d_.cat = d_.cat;
        other.clear();
    }

    error& error::operator=(const error& other)
    {
        clear();
        code_ = other.code_;
        if (other.flag_ == by_desc)
        {
            d_.desc = make_error_descriptor(*other.d_.desc);
        }
        else
        {
            d_.cat = other.d_.cat;
        }
        return *this;
    }

    error& error::operator=(error&& other) noexcept
    {
        code_ = other.code_;
        d_ = other.d_;
        flag_ = other.flag_;

        other.flag_ = by_cat;
        other.d_.cat = d_.cat;
        other.clear();

        return *this;
    }

    error::~error() noexcept
    {
        if (flag_ == by_desc)
            try_free_desc();
    }

    int error::value() const noexcept
    {
        return code_;
    }

    std::error_category const& error::category() const noexcept
    {
        if (flag_ == by_desc)
            return *d_.desc->category;
        return *d_.cat;
    }

    std::error_condition error::default_error_condition() const noexcept
    {
        return category().default_error_condition(code_);
    }

    std::string error::description() const noexcept
    {
        return category().message(code_);
    }

    std::string error::comment() const noexcept
    {
        if (flag_ == by_desc)
            return d_.desc->comment;
        return {};
    }

    bool error::commented() const noexcept
    {
        return (flag_ == by_desc) && (!d_.desc->comment.empty());
    }

    const char* error::what() const noexcept
    {
        NX_THREAD_LOCAL std::string s_what_cache;
        s_what_cache.clear();

        std::stringstream ss;
        ss << "[" << category().name() << " error " << code_ << "] - ";
        ss << description();

        if (commented())
        {
            ss << std::endl << "Comment: " << comment();
        }

        if (located())
        {
            auto location = d_.desc->location;

            ss << std::endl << "Location: " << location.short_link();
            ss << std::endl << "\tFunction: " << location.function();
            ss << std::endl << "\tFile: " << location.file();
            ss << std::endl << "\tLine: " << location.line();
            if constexpr(nx::source_location::has_builtin_column)
            {
                ss << std::endl << "\tColumn: " << location.column();
            }
        }
        s_what_cache = ss.str();
        return s_what_cache.c_str();
    }

    nx::source_location error::where() const noexcept
    {
        if (flag_ == by_desc)
            return d_.desc->location;
        return g_undefined_location;
    }

    bool error::located() const noexcept
    {
        return (flag_ == by_desc) && (d_.desc->location != g_undefined_location);
    }

    error::operator bool() const noexcept
    {
        return static_cast<bool>(code_);
    }

    error error::operator()(std::string_view comment, const nx::source_location& loc) const noexcept
    {
        auto copy = get_copy();
        copy.promote_desc();
        copy.d_.desc->comment = comment;
        copy.d_.desc->location = loc;
        return copy;
    }

    void error::clear() noexcept
    {
        try_free_desc();
        d_.cat = const_cast<std::error_category *>(&std::system_category());
        code_ = 0;
    }

    void explain(error const& err, FILE * out) noexcept
    {
        // std::cerr << err.what() << std::endl;
        fprintf(out, "%s\n", err.what());
    }
}
