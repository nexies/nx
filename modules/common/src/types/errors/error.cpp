//
// Created by nexie on 4/1/2026.
//

#include <nx/common/types/errors/error.hpp>
#include <sstream>

namespace {
    const nx::named_error_category g_nx_error_category { "nx" };
}

namespace nx {

    const std::error_category& error_category() noexcept {
        return g_nx_error_category;
    }

    // ── private helpers ──────────────────────────────────────────────────────

    error error::get_copy() const
    {
        if (flag_ == by_cat)
            return error(*d_.cat, code_, {}, g_undefined_location);
        return error(*d_.desc->category, code_, d_.desc->comment, d_.desc->location);
    }

    error_descriptor* error::try_alloc_desc() noexcept
    {
        if (flag_ == by_desc)
            return d_.desc;

        auto* tmp = make_error_descriptor(g_undefined_location, {});
        if (tmp) {
            tmp->category = d_.cat;
            d_.desc = tmp;
            flag_ = by_desc;
        }
        return tmp;
    }

    void error::try_free_desc() noexcept
    {
        if (flag_ == by_cat)
            return;
        auto* cat = d_.desc->category;
        free_error_descriptor(d_.desc);
        d_.cat = cat;
        flag_ = by_cat;
    }

    void error::promote_desc() noexcept
    {
        if (flag_ == by_desc)
            return;
        auto* cat = d_.cat;
        if (!try_alloc_desc())
            d_.cat = cat;  // restore on alloc failure
        else
            d_.desc->category = cat;
    }

    // ── protected constructor for subclasses ─────────────────────────────────

    error::error(const std::error_category& cat, int code, std::string_view message,
                 const nx::source_location& loc) noexcept
        : code_(code), flag_(by_cat), d_ {}
    {
        d_.cat = const_cast<std::error_category*>(&cat);
        auto* desc = try_alloc_desc();
        if (!desc)
            return;
        flag_ = by_desc;
        d_.desc->category = const_cast<std::error_category*>(&cat);
        d_.desc->location = loc;
        d_.desc->comment  = message;
    }

    // ── public constructors ──────────────────────────────────────────────────

    error::error() noexcept
        : code_(0), flag_(by_cat), d_ {}
    {
        d_.cat = const_cast<std::error_category*>(&error_category());
    }

    error::error(std::string_view message, const nx::source_location& loc) noexcept
        : error(error_category(), 1, message, loc)
    {}

    error::error(int code, std::string_view message, const nx::source_location& loc) noexcept
        : error(error_category(), code, message, loc)
    {}

    error::error(const std::error_code& code) noexcept
        : code_(code.value()), flag_(by_cat), d_ {}
    {
        d_.cat = const_cast<std::error_category*>(&code.category());
    }

    error::error(const std::error_code& code, std::string_view comment,
                 const nx::source_location& loc) noexcept
        : error(code.category(), code.value(), comment, loc)
    {}

    error::error(const error& other)
        : code_(other.code_), flag_(by_cat), d_ {}
    {
        if (other.flag_ == by_desc) {
            auto* desc = make_error_descriptor(*other.d_.desc);
            if (desc) {
                flag_ = by_desc;
                d_.desc = desc;
                return;
            }
        }
        d_.cat = other.flag_ == by_desc ? other.d_.desc->category : other.d_.cat;
    }

    error::error(error&& other) noexcept
        : code_(other.code_), flag_(other.flag_), d_(other.d_)
    {
        other.code_  = 0;
        other.flag_  = by_cat;
        other.d_.cat = const_cast<std::error_category*>(&error_category());
    }

    error& error::operator=(const error& other)
    {
        if (this == &other)
            return *this;
        error tmp(other);
        *this = std::move(tmp);
        return *this;
    }

    error& error::operator=(error&& other) noexcept
    {
        if (this == &other)
            return *this;
        try_free_desc();
        code_  = other.code_;
        flag_  = other.flag_;
        d_     = other.d_;
        other.code_  = 0;
        other.flag_  = by_cat;
        other.d_.cat = const_cast<std::error_category*>(&error_category());
        return *this;
    }

    error::~error() noexcept
    {
        try_free_desc();
    }

    // ── observers ────────────────────────────────────────────────────────────

    int error::value() const noexcept
    {
        return code_;
    }

    std::error_category const& error::category() const noexcept
    {
        return flag_ == by_desc ? *d_.desc->category : *d_.cat;
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
        return flag_ == by_desc ? d_.desc->comment : std::string{};
    }

    bool error::commented() const noexcept
    {
        return flag_ == by_desc && !d_.desc->comment.empty();
    }

    const char* error::what() const noexcept
    {
        NX_THREAD_LOCAL std::string s_what_cache;
        s_what_cache.clear();

        std::stringstream ss;
        ss << "[" << category().name() << " " << code_ << "]";

        auto desc = description();
        auto cmt  = comment();

        if (!desc.empty() && !cmt.empty())
            ss << " " << desc << ": " << cmt;
        else if (!desc.empty())
            ss << " " << desc;
        else if (!cmt.empty())
            ss << " " << cmt;

        if (located()) {
            auto loc = d_.desc->location;
            ss << "\n  at " << loc.short_link();
            ss << " in " << loc.function();
        }

        s_what_cache = ss.str();
        return s_what_cache.c_str();
    }

    nx::source_location error::where() const noexcept
    {
        return flag_ == by_desc ? d_.desc->location : g_undefined_location;
    }

    bool error::located() const noexcept
    {
        return flag_ == by_desc && d_.desc->location != g_undefined_location;
    }

    error::operator bool() const noexcept
    {
        return code_ != 0;
    }

    // ── mutations ────────────────────────────────────────────────────────────

    error error::operator()(std::string_view cmt, const nx::source_location& loc) const noexcept
    {
        auto copy = get_copy();
        copy.promote_desc();
        if (copy.flag_ == by_desc) {
            copy.d_.desc->comment  = cmt;
            copy.d_.desc->location = loc;
        }
        return copy;
    }

    void error::clear() noexcept
    {
        try_free_desc();
        d_.cat = const_cast<std::error_category*>(&error_category());
        code_ = 0;
    }

    // ── comparisons ─────────────────────────────────────────────────────────

    bool error::equivalent(const error& other) const noexcept
    {
        return code_ == other.code_ && &category() == &other.category();
    }

    bool error::identical(const error& other) const noexcept
    {
        return equivalent(other)
            && comment() == other.comment()
            && where()   == other.where();
    }

    bool error::operator==(const error& other) const noexcept { return equivalent(other); }
    bool error::operator!=(const error& other) const noexcept { return !equivalent(other); }

    // ── utilities ────────────────────────────────────────────────────────────

    void explain(error const& err, FILE* out) noexcept
    {
        fprintf(out, "%s\n", err.what());
    }

} // namespace nx
