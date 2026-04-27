//
// Created by nexie on 4/27/2026.
//

#ifndef NX_COMMON_UTF8_HPP
#define NX_COMMON_UTF8_HPP

#include <nx/common/helpers.hpp>
#include <nx/common/types/result.hpp>

#include <string_view>
#include <string>

namespace nx::utf8
{

// ─────────────────────────────────────────────────────────────────────────────
// detail — core UTF-8 / grapheme algorithms (header-only, all inline)
// ─────────────────────────────────────────────────────────────────────────────
namespace detail
{
    struct decoded {
        char32_t cp;    // unicode codepoint
        int      bytes; // bytes consumed
    };

    // Decode one UTF-8 codepoint starting at p (exclusive end at `end`).
    // Returns decoded or an error on invalid byte sequence.
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

        // Overlong encoding
        static constexpr char32_t min_cp[] = { 0, 0, 0x80, 0x800, 0x10000 };
        if (cp < min_cp[n])
            return nx::err::invalid_argument("utf8: overlong encoding");

        // Surrogates
        if (cp >= 0xD800 && cp <= 0xDFFF)
            return nx::err::invalid_argument("utf8: surrogate codepoint");

        if (cp > 0x10FFFF)
            return nx::err::invalid_argument("utf8: codepoint out of range");

        return decoded{ cp, n };
    }

    // Returns true if `cp` extends the preceding grapheme cluster
    // (i.e. it is a combining mark, variation selector, ZWJ, etc.)
    // Covers common cases per Unicode UAX#29.
    inline bool
    is_grapheme_extend(char32_t cp) noexcept
    {
        if (cp == 0x200D) return true;  // Zero Width Joiner

        // Combining Diacritical Marks
        if (cp >= 0x0300 && cp <= 0x036F) return true;
        // Combining Diacritical Marks Extended
        if (cp >= 0x1AB0 && cp <= 0x1AFF) return true;
        // Combining Diacritical Marks Supplement
        if (cp >= 0x1DC0 && cp <= 0x1DFF) return true;
        // Combining Diacritical Marks for Symbols
        if (cp >= 0x20D0 && cp <= 0x20FF) return true;
        // Combining Half Marks
        if (cp >= 0xFE20 && cp <= 0xFE2F) return true;

        // Variation Selectors
        if (cp >= 0xFE00 && cp <= 0xFE0F) return true;
        // Variation Selectors Supplement
        if (cp >= 0xE0100 && cp <= 0xE01EF) return true;

        // Emoji modifiers (skin tones U+1F3FB..U+1F3FF)
        if (cp >= 0x1F3FB && cp <= 0x1F3FF) return true;

        // Tags (used in emoji flag sequences)
        if (cp >= 0xE0020 && cp <= 0xE007F) return true;

        return false;
    }

    // Returns a pointer to the first byte of the NEXT grapheme cluster
    // after the one starting at `p`. Returns error on invalid UTF-8 at `p`.
    // If the very first codepoint at `p` is invalid, returns error so the
    // caller can decide how to handle it (e.g. skip one byte).
    inline result<const char *>
    cluster_end(const char * p, const char * end) noexcept
    {
        // Decode the base codepoint (errors propagate to the caller)
        auto first = decode(p, end);
        if (!first)
            return first.error();

        const char * cur      = p + first->bytes;
        char32_t     prev_cp  = first->cp;

        // Regional Indicator pair (flag emoji): two consecutive RIs = one cluster
        const bool base_is_ri = (prev_cp >= 0x1F1E6 && prev_cp <= 0x1F1FF);

        while (cur < end) {
            auto r = decode(cur, end);
            if (!r)
                break; // invalid byte ends the cluster

            const char32_t cp = r->cp;

            // ZWJ sequence: current ZWJ + following codepoint = one cluster
            if (prev_cp == 0x200D) {
                cur     += r->bytes;
                prev_cp  = cp;
                continue;
            }

            // Regional Indicator pair
            if (base_is_ri && (cp >= 0x1F1E6 && cp <= 0x1F1FF)) {
                cur += r->bytes;
                break; // only two RIs per cluster
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
// grapheme — a single grapheme cluster (one or more codepoints)
// ─────────────────────────────────────────────────────────────────────────────
class grapheme
{
public:
    // Raw UTF-8 bytes of this cluster
    NX_NODISCARD std::string_view
    bytes() const noexcept;

    // All codepoints in this cluster decoded to UTF-32
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
// grapheme_iterator — forward iterator over grapheme clusters
// ─────────────────────────────────────────────────────────────────────────────
class grapheme_iterator
{
public:
    using value_type        = grapheme;
    using difference_type   = std::ptrdiff_t;
    using iterator_category = std::forward_iterator_tag;

    // Returns the current grapheme, or an error if the byte sequence is invalid.
    NX_NODISCARD result<value_type>
    operator*() const;

    grapheme_iterator & operator++();
    grapheme_iterator   operator++(int);

    bool operator==(const grapheme_iterator & other) const;
    bool operator!=(const grapheme_iterator & other) const;

private:
    friend class view;

    // `cluster_end` = first byte of the NEXT cluster (cached on construction
    // and after each increment so operator* is O(1)).
    // When current_ == end_, the iterator is the past-the-end sentinel.
    grapheme_iterator(const char * current, const char * end) noexcept;

    const char * current_;      // start of the current cluster
    const char * cluster_end_;  // one-past-end of current cluster (= start of next)
    const char * end_;          // one-past-end of the whole buffer
};


// ─────────────────────────────────────────────────────────────────────────────
// view — non-owning UTF-8 string view with grapheme-cluster iteration
// ─────────────────────────────────────────────────────────────────────────────
class view
{
public:
    explicit view(std::string_view text) noexcept;

    NX_NODISCARD grapheme_iterator begin() const noexcept;
    NX_NODISCARD grapheme_iterator end()   const noexcept;

    NX_NODISCARD bool        empty() const noexcept;
    NX_NODISCARD std::size_t bytes() const noexcept;  // byte count, O(1)
    NX_NODISCARD std::size_t size()  const noexcept;  // grapheme count, O(n)

private:
    std::string_view text_;
};

} // namespace nx::utf8

#endif //NX_COMMON_UTF8_HPP
