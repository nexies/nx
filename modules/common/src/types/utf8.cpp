//
// Created by nexie on 4/27/2026.
//

#include <nx/common/types/utf8.hpp>
#include <nx/string_view.hpp>

namespace nx { namespace utf8 {

// ─────────────────────────────────────────────────────────────────────────────
// grapheme
// ─────────────────────────────────────────────────────────────────────────────

nx::string_view
grapheme::bytes() const noexcept
{
    return nx::string_view(begin_, static_cast<std::size_t>(end_ - begin_));
}

std::u32string
grapheme::codepoints() const noexcept
{
    std::u32string out;
    const char * p = begin_;
    while (p < end_) {
        auto r = detail::decode(p, end_);
        if (!r)
            break;
        out += r->cp;
        p   += r->bytes;
    }
    return out;
}


// ─────────────────────────────────────────────────────────────────────────────
// grapheme_iterator
// ─────────────────────────────────────────────────────────────────────────────

grapheme_iterator::grapheme_iterator(const char * current, const char * end) noexcept
    : current_(current)
    , cluster_end_(current)
    , end_(end)
{
    if (current_ < end_) {
        auto r = detail::cluster_end(current_, end_);
        cluster_end_ = r ? *r : current_ + 1;
    }
}

result<grapheme_iterator::value_type>
grapheme_iterator::operator*() const
{
    if (current_ >= end_)
        return nx::err::invalid_argument("utf8: dereference of end iterator");

    auto r = detail::decode(current_, cluster_end_);
    if (!r)
        return r.error();

    return grapheme(current_, cluster_end_);
}

grapheme_iterator &
grapheme_iterator::operator++()
{
    current_ = cluster_end_;

    if (current_ < end_) {
        auto r = detail::cluster_end(current_, end_);
        cluster_end_ = r ? *r : current_ + 1;
    } else {
        cluster_end_ = end_;
    }

    return *this;
}

grapheme_iterator
grapheme_iterator::operator++(int)
{
    grapheme_iterator tmp = *this;
    ++(*this);
    return tmp;
}

bool
grapheme_iterator::operator==(const grapheme_iterator & other) const
{
    return current_ == other.current_;
}

bool
grapheme_iterator::operator!=(const grapheme_iterator & other) const
{
    return current_ != other.current_;
}


// ─────────────────────────────────────────────────────────────────────────────
// view
// ─────────────────────────────────────────────────────────────────────────────

view::view(nx::string_view text) noexcept
    : text_(text)
{}

grapheme_iterator
view::begin() const noexcept
{
    return { text_.data(), text_.data() + text_.size() };
}

grapheme_iterator
view::end() const noexcept
{
    const char * e = text_.data() + text_.size();
    return { e, e };
}

bool
view::empty() const noexcept
{
    return text_.empty();
}

std::size_t
view::bytes() const noexcept
{
    return text_.size();
}

std::size_t
view::size() const noexcept
{
    std::size_t count = 0;
    for (auto it = begin(); it != end(); ++it)
        ++count;
    return count;
}

} } // namespace nx::utf8
