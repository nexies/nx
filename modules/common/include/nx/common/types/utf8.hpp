//
// Created by nexie on 4/27/2026.
//

#ifndef NX_COMMON_UTF8_HPP
#define NX_COMMON_UTF8_HPP

#include <nx/common/helpers.hpp>
#include <nx/common/types/result.hpp>
#include <nx/string_view.hpp>

#include <string>

// C++11-compatible nested namespace (C++17 syntax: namespace nx::utf8)
namespace nx { namespace utf8 {

// ─────────────────────────────────────────────────────────────────────────────
// detail — core UTF-8 / grapheme algorithms (header-only, all inline)
// ─────────────────────────────────────────────────────────────────────────────
namespace detail
{
    struct decoded {
        char32_t cp;    // unicode codepoint
        int      bytes; // bytes consumed
    };

    inline result<decoded>
    decode(const char * p, const char * end) noexcept
    {
        if (p >= end)
            return nx::err::invalid_argument("utf8: unexpected end of input");

        const auto b0 = static_cast<unsigned char>(*p);

        int      n;
        char32_t cp;

        if      (b0 < 0x80)              { return decoded{ static_cast<char32_t>(b0), 1 }; }
        else if ((b0 & 0xE0) == 0xC0)   { n = 2; cp = b0 & 0x1F; }
        else if ((b0 & 0xF0) == 0xE0)   { n = 3; cp = b0 & 0x0F; }
        else if ((b0 & 0xF8) == 0xF0)   { n = 4; cp = b0 & 0x07; }
        else
            return nx::err::invalid_argument("utf8: invalid lead byte");

        if (p + n > end)
            return nx::err::invalid_argument("utf8: sequence truncated");

        for (int i = 1; i < n; ++i) {
            const auto b = static_cast<unsigned char>(p[i]);
            if ((b & 0xC0) != 0x80)
                return nx::err::invalid_argument("utf8: invalid continuation byte");
            cp = (cp << 6) | (b & 0x3F);
        }

        static constexpr char32_t min_cp[] = { 0, 0, 0x80, 0x800, 0x10000 };
        if (cp < min_cp[n])
            return nx::err::invalid_argument("utf8: overlong encoding");

        if (cp >= 0xD800 && cp <= 0xDFFF)
            return nx::err::invalid_argument("utf8: surrogate codepoint");

        if (cp > 0x10FFFF)
            return nx::err::invalid_argument("utf8: codepoint out of range");

        return decoded{ cp, n };
    }

    inline bool
    is_grapheme_extend(char32_t cp) noexcept
    {
        if (cp == 0x200D) return true;
        if (cp >= 0x0300 && cp <= 0x036F) return true;
        if (cp >= 0x1AB0 && cp <= 0x1AFF) return true;
        if (cp >= 0x1DC0 && cp <= 0x1DFF) return true;
        if (cp >= 0x20D0 && cp <= 0x20FF) return true;
        if (cp >= 0xFE20 && cp <= 0xFE2F) return true;
        if (cp >= 0xFE00 && cp <= 0xFE0F) return true;
        if (cp >= 0xE0100 && cp <= 0xE01EF) return true;
        if (cp >= 0x1F3FB && cp <= 0x1F3FF) return true;
        if (cp >= 0xE0020 && cp <= 0xE007F) return true;
        return false;
    }

    inline result<const char *>
    cluster_end(const char * p, const char * end) noexcept
    {
        auto first = decode(p, end);
        if (!first)
            return first.error();

        const char * cur      = p + first->bytes;
        char32_t     prev_cp  = first->cp;

        const bool base_is_ri = (prev_cp >= 0x1F1E6 && prev_cp <= 0x1F1FF);

        while (cur < end) {
            auto r = decode(cur, end);
            if (!r)
                break;

            const char32_t cp = r->cp;

            if (prev_cp == 0x200D) {
                cur     += r->bytes;
                prev_cp  = cp;
                continue;
            }

            if (base_is_ri && (cp >= 0x1F1E6 && cp <= 0x1F1FF)) {
                cur += r->bytes;
                break;
            }

            if (is_grapheme_extend(cp)) {
                cur     += r->bytes;
                prev_cp  = cp;
                continue;
            }

            break;
        }

        return cur;
    }

} // namespace detail


// ─────────────────────────────────────────────────────────────────────────────
// grapheme — a single grapheme cluster
// ─────────────────────────────────────────────────────────────────────────────
class grapheme
{
public:
    NX_NODISCARD nx::string_view
    bytes() const noexcept;

    NX_NODISCARD std::u32string
    codepoints() const noexcept;

private:
    friend class grapheme_iterator;

    constexpr
    grapheme() noexcept :
        begin_ { nullptr}, end_ { nullptr } {}

    constexpr
    grapheme(const char * begin, const char * end) noexcept
        : begin_(begin), end_(end) {}

    const char * begin_;
    const char * end_;
};


// ─────────────────────────────────────────────────────────────────────────────
// grapheme_iterator
// ─────────────────────────────────────────────────────────────────────────────
class grapheme_iterator
{
public:
    using value_type        = grapheme;
    using difference_type   = std::ptrdiff_t;
    using iterator_category = std::forward_iterator_tag;

    NX_NODISCARD result<value_type>
    operator*() const;

    grapheme_iterator & operator++();
    grapheme_iterator   operator++(int);

    bool operator==(const grapheme_iterator & other) const;
    bool operator!=(const grapheme_iterator & other) const;

private:
    friend class view;

    grapheme_iterator(const char * current, const char * end) noexcept;

    const char * current_;
    const char * cluster_end_;
    const char * end_;
};


// ─────────────────────────────────────────────────────────────────────────────
// view — non-owning UTF-8 string view with grapheme-cluster iteration
// ─────────────────────────────────────────────────────────────────────────────
class view
{
public:
    explicit view(nx::string_view text) noexcept;

    NX_NODISCARD grapheme_iterator begin() const noexcept;
    NX_NODISCARD grapheme_iterator end()   const noexcept;

    NX_NODISCARD bool        empty() const noexcept;
    NX_NODISCARD std::size_t bytes() const noexcept;
    NX_NODISCARD std::size_t size()  const noexcept;

private:
    nx::string_view text_;
};

} } // namespace nx::utf8

#endif //NX_COMMON_UTF8_HPP
