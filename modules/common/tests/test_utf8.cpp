//
// Created by nexie on 4/27/2026.
//

#include <nx/common/types/utf8.hpp>

#include <catch2/catch_test_macros.hpp>

#include <string>
#include <vector>

// ─────────────────────────────────────────────────────────────────────────────
// detail::decode
// ─────────────────────────────────────────────────────────────────────────────

TEST_CASE("decode: ASCII single byte", "[utf8][decode]")
{
    const char buf[] = "A";
    auto r = nx::utf8::detail::decode(buf, buf + 1);
    REQUIRE(r);
    if (r)
    {
        auto v = r.value();
        REQUIRE(v.cp == U'A');
        REQUIRE(v.bytes == 1);
    }
}

TEST_CASE("decode: 2-byte sequence U+00E9 (é)", "[utf8][decode]")
{
    // U+00E9 = 0xC3 0xA9
    const unsigned char raw[] = { 0xC3, 0xA9 };
    const char * p = reinterpret_cast<const char *>(raw);
    auto r = nx::utf8::detail::decode(p, p + 2);
    REQUIRE(r);
    REQUIRE(r.value().cp == U'é');
    REQUIRE(r.value().bytes == 2);
}

TEST_CASE("decode: 3-byte sequence U+4E2D (中)", "[utf8][decode]")
{
    // U+4E2D = 0xE4 0xB8 0xAD
    const unsigned char raw[] = { 0xE4, 0xB8, 0xAD };
    const char * p = reinterpret_cast<const char *>(raw);
    auto r = nx::utf8::detail::decode(p, p + 3);
    REQUIRE(r);
    REQUIRE(r->cp == U'中');
    REQUIRE(r->bytes == 3);
}

TEST_CASE("decode: 4-byte sequence U+1F600 (😀)", "[utf8][decode]")
{
    // U+1F600 = 0xF0 0x9F 0x98 0x80
    const unsigned char raw[] = { 0xF0, 0x9F, 0x98, 0x80 };
    const char * p = reinterpret_cast<const char *>(raw);
    auto r = nx::utf8::detail::decode(p, p + 4);
    REQUIRE(r);
    REQUIRE(r->cp == U'\U0001F600');
    REQUIRE(r->bytes == 4);
}

TEST_CASE("decode: empty input returns error", "[utf8][decode]")
{
    const char * p = "A";
    auto r = nx::utf8::detail::decode(p, p); // end == begin
    REQUIRE_FALSE(r);
    // nx::explain(r.error());
}

TEST_CASE("decode: truncated sequence returns error", "[utf8][decode]")
{
    const unsigned char raw[] = { 0xC3 }; // missing continuation
    const char * p = reinterpret_cast<const char *>(raw);
    auto r = nx::utf8::detail::decode(p, p + 1);
    REQUIRE_FALSE(r);
    // nx::explain(r.error());
}

TEST_CASE("decode: invalid continuation byte returns error", "[utf8][decode]")
{
    const unsigned char raw[] = { 0xC3, 0x20 }; // 0x20 is not a continuation byte
    const char * p = reinterpret_cast<const char *>(raw);
    auto r = nx::utf8::detail::decode(p, p + 2);
    REQUIRE_FALSE(r);
    // nx::explain(r.error());
}

TEST_CASE("decode: overlong encoding returns error", "[utf8][decode]")
{
    // Overlong 2-byte encoding of U+0041 ('A')
    const unsigned char raw[] = { 0xC1, 0x81 };
    const char * p = reinterpret_cast<const char *>(raw);
    auto r = nx::utf8::detail::decode(p, p + 2);
    REQUIRE_FALSE(r);
    // nx::explain(r.error());
}

TEST_CASE("decode: surrogate codepoint returns error", "[utf8][decode]")
{
    // U+D800 in UTF-8 would be 0xED 0xA0 0x80 (illegal)
    const unsigned char raw[] = { 0xED, 0xA0, 0x80 };
    const char * p = reinterpret_cast<const char *>(raw);
    auto r = nx::utf8::detail::decode(p, p + 3);
    REQUIRE_FALSE(r);
}

TEST_CASE("decode: invalid lead byte returns error", "[utf8][decode]")
{
    const unsigned char raw[] = { 0xFF };
    const char * p = reinterpret_cast<const char *>(raw);
    auto r = nx::utf8::detail::decode(p, p + 1);
    REQUIRE_FALSE(r);
}

// ─────────────────────────────────────────────────────────────────────────────
// grapheme::bytes / grapheme::codepoints
// ─────────────────────────────────────────────────────────────────────────────

TEST_CASE("grapheme: bytes() returns raw UTF-8 of ASCII char", "[utf8][grapheme]")
{
    nx::utf8::view v("hello");
    auto it = v.begin();
    auto g = *it;
    REQUIRE(g);
    REQUIRE(g->bytes() == "h");
}

TEST_CASE("grapheme: codepoints() returns single codepoint for ASCII", "[utf8][grapheme]")
{
    nx::utf8::view v("Z");
    auto g = *v.begin();
    REQUIRE(g);
    auto cps = g->codepoints();
    REQUIRE(cps.size() == 1);
    REQUIRE(cps[0] == U'Z');
}

TEST_CASE("grapheme: codepoints() returns multiple codepoints for combining cluster", "[utf8][grapheme]")
{
    // U+0065 (e) + U+0301 (combining acute) → "é" as two codepoints
    const unsigned char raw[] = { 0x65, 0xCC, 0x81 };
    std::string s(reinterpret_cast<const char *>(raw), 3);

    nx::utf8::view v(s);
    auto g = *v.begin();
    REQUIRE(g);

    auto cps = g->codepoints();
    REQUIRE(cps.size() == 2);
    REQUIRE(cps[0] == U'e');
    REQUIRE(cps[1] == U'́');
}

// ─────────────────────────────────────────────────────────────────────────────
// grapheme_iterator
// ─────────────────────────────────────────────────────────────────────────────

TEST_CASE("grapheme_iterator: iterates ASCII string one grapheme at a time", "[utf8][iterator]")
{
    nx::utf8::view v("abc");
    std::vector<std::string> clusters;
    for (auto it = v.begin(); it != v.end(); ++it) {
        auto g = *it;
        REQUIRE(g);
        clusters.push_back(std::string(g->bytes()));
    }
    REQUIRE(clusters.size() == 3);
    REQUIRE(clusters[0] == "a");
    REQUIRE(clusters[1] == "b");
    REQUIRE(clusters[2] == "c");
}

TEST_CASE("grapheme_iterator: empty string produces no iterations", "[utf8][iterator]")
{
    nx::utf8::view v("");
    REQUIRE(v.begin() == v.end());
}

TEST_CASE("grapheme_iterator: post-increment works", "[utf8][iterator]")
{
    nx::utf8::view v("ab");
    auto it = v.begin();
    auto first = it++;
    auto g1 = *first;
    auto g2 = *it;
    REQUIRE(g1);
    REQUIRE(g2);
    REQUIRE(g1->bytes() == "a");
    REQUIRE(g2->bytes() == "b");
}

TEST_CASE("grapheme_iterator: multi-byte characters iterated correctly", "[utf8][iterator]")
{
    // "中文" = U+4E2D U+6587
    const unsigned char raw[] = { 0xE4, 0xB8, 0xAD, 0xE6, 0x96, 0x87 };
    std::string s(reinterpret_cast<const char *>(raw), 6);

    nx::utf8::view v(s);
    std::vector<std::u32string> cps;
    for (auto it = v.begin(); it != v.end(); ++it) {
        auto g = *it;
        REQUIRE(g);
        cps.push_back(g->codepoints());
    }
    REQUIRE(cps.size() == 2);
    REQUIRE(cps[0][0] == U'中');
    REQUIRE(cps[1][0] == U'文');
}

// ─────────────────────────────────────────────────────────────────────────────
// Grapheme cluster segmentation
// ─────────────────────────────────────────────────────────────────────────────

TEST_CASE("cluster: combining diacritic merges with base character", "[utf8][cluster]")
{
    // U+0065 'e' + U+0301 combining acute → one grapheme cluster
    const unsigned char raw[] = { 0x65, 0xCC, 0x81 };
    std::string s(reinterpret_cast<const char *>(raw), 3);

    nx::utf8::view v(s);
    REQUIRE(v.size() == 1);
}

TEST_CASE("cluster: ZWJ sequence forms single grapheme cluster", "[utf8][cluster]")
{
    // U+1F468 (man) + U+200D (ZWJ) + U+1F469 (woman) → one cluster
    const unsigned char raw[] = {
        0xF0, 0x9F, 0x91, 0xA8, // U+1F468
        0xE2, 0x80, 0x8D,       // U+200D ZWJ
        0xF0, 0x9F, 0x91, 0xA9  // U+1F469
    };
    std::string s(reinterpret_cast<const char *>(raw), sizeof(raw));

    nx::utf8::view v(s);
    REQUIRE(v.size() == 1);
}

TEST_CASE("cluster: chained ZWJ family emoji is one grapheme cluster ('👨‍👩‍👧‍👦')", "[utf8][cluster]")
{
    // 👨‍👩‍👧‍👦 = U+1F468 ZWJ U+1F469 ZWJ U+1F467 ZWJ U+1F466
    // 25 bytes, must count as exactly 1 cluster
    const unsigned char raw[] = {
        0xF0, 0x9F, 0x91, 0xA8, // U+1F468 man
        0xE2, 0x80, 0x8D,       // U+200D  ZWJ
        0xF0, 0x9F, 0x91, 0xA9, // U+1F469 woman
        0xE2, 0x80, 0x8D,       // U+200D  ZWJ
        0xF0, 0x9F, 0x91, 0xA7, // U+1F467 girl
        0xE2, 0x80, 0x8D,       // U+200D  ZWJ
        0xF0, 0x9F, 0x91, 0xA6  // U+1F466 boy
    };
    std::string s(reinterpret_cast<const char *>(raw), sizeof(raw));

    nx::utf8::view v(s);
    REQUIRE(v.bytes() == 25);
    REQUIRE(v.size() == 1);

    auto g = *v.begin();
    REQUIRE(g);
    REQUIRE(g->bytes().size() == 25);

    // All 7 codepoints decoded
    auto cps = g->codepoints();
    REQUIRE(cps.size() == 7);
    REQUIRE(cps[0] == U'\U0001F468'); // man
    REQUIRE(cps[1] == U'‍');     // ZWJ
    REQUIRE(cps[2] == U'\U0001F469'); // woman
    REQUIRE(cps[3] == U'‍');     // ZWJ
    REQUIRE(cps[4] == U'\U0001F467'); // girl
    REQUIRE(cps[5] == U'‍');     // ZWJ
    REQUIRE(cps[6] == U'\U0001F466'); // boy
}

TEST_CASE("cluster: family emoji  followed by ASCII is two clusters ('👨‍👩‍👧‍👦' + '!')", "[utf8][cluster]")
{
    // 👨‍👩‍👧‍👦 + '!' = 2 clusters
    const unsigned char raw[] = {
        0xF0, 0x9F, 0x91, 0xA8,
        0xE2, 0x80, 0x8D,
        0xF0, 0x9F, 0x91, 0xA9,
        0xE2, 0x80, 0x8D,
        0xF0, 0x9F, 0x91, 0xA7,
        0xE2, 0x80, 0x8D,
        0xF0, 0x9F, 0x91, 0xA6,
        0x21 // '!'
    };
    std::string s(reinterpret_cast<const char *>(raw), sizeof(raw));

    nx::utf8::view v(s);
    REQUIRE(v.size() == 2);

    auto it = v.begin();
    auto family = *it;
    REQUIRE(family);
    REQUIRE(family->bytes().size() == 25);

    ++it;
    auto bang = *it;
    REQUIRE(bang);
    REQUIRE(bang->bytes() == "!");
}

TEST_CASE("cluster: regional indicator pair forms one grapheme cluster (flag)", "[utf8][cluster]")
{
    // U+1F1FA (🇺) + U+1F1F8 (🇸) → US flag, one cluster
    const unsigned char raw[] = {
        0xF0, 0x9F, 0x87, 0xBA, // U+1F1FA
        0xF0, 0x9F, 0x87, 0xB8  // U+1F1F8
    };
    std::string s(reinterpret_cast<const char *>(raw), sizeof(raw));

    nx::utf8::view v(s);
    REQUIRE(v.size() == 1);
}

TEST_CASE("cluster: three regional indicators are two clusters", "[utf8][cluster]")
{
    // RI + RI + RI = one flag + one lone RI
    const unsigned char raw[] = {
        0xF0, 0x9F, 0x87, 0xBA, // U+1F1FA
        0xF0, 0x9F, 0x87, 0xB8, // U+1F1F8
        0xF0, 0x9F, 0x87, 0xBA  // U+1F1FA
    };
    std::string s(reinterpret_cast<const char *>(raw), sizeof(raw));

    nx::utf8::view v(s);
    REQUIRE(v.size() == 2);
}

TEST_CASE("cluster: emoji with skin tone modifier is one cluster", "[utf8][cluster]")
{
    // U+1F44D (thumbs up) + U+1F3FB (light skin tone modifier)
    const unsigned char raw[] = {
        0xF0, 0x9F, 0x91, 0x8D, // U+1F44D
        0xF0, 0x9F, 0x8F, 0xBB  // U+1F3FB
    };
    std::string s(reinterpret_cast<const char *>(raw), sizeof(raw));

    nx::utf8::view v(s);
    REQUIRE(v.size() == 1);
}

// ─────────────────────────────────────────────────────────────────────────────
// view
// ─────────────────────────────────────────────────────────────────────────────

TEST_CASE("view: empty() is true for empty string", "[utf8][view]")
{
    nx::utf8::view v("");
    REQUIRE(v.empty());
}

TEST_CASE("view: empty() is false for non-empty string", "[utf8][view]")
{
    nx::utf8::view v("x");
    REQUIRE_FALSE(v.empty());
}

TEST_CASE("view: bytes() returns byte count not grapheme count", "[utf8][view]")
{
    // "中" is 3 bytes but 1 grapheme
    const unsigned char raw[] = { 0xE4, 0xB8, 0xAD };
    std::string s(reinterpret_cast<const char *>(raw), 3);

    nx::utf8::view v(s);
    REQUIRE(v.bytes() == 3);
    REQUIRE(v.size() == 1);
}

TEST_CASE("view: size() counts ASCII graphemes correctly", "[utf8][view]")
{
    nx::utf8::view v("hello");
    REQUIRE(v.size() == 5);
    REQUIRE(v.bytes() == 5);
}

TEST_CASE("view: range-for loop iterates all graphemes", "[utf8][view]")
{
    nx::utf8::view v("hi!");
    std::size_t count = 0;
    for (auto && g : v) {
        (void)g;
        ++count;
    }
    REQUIRE(count == 3);
}

TEST_CASE("view: mixed ASCII and multi-byte size()", "[utf8][view]")
{
    // "a中b" = 1 + 3 + 1 = 5 bytes, 3 graphemes
    const unsigned char raw[] = { 0x61, 0xE4, 0xB8, 0xAD, 0x62 };
    std::string s(reinterpret_cast<const char *>(raw), sizeof(raw));

    nx::utf8::view v(s);
    REQUIRE(v.bytes() == 5);
    REQUIRE(v.size() == 3);
}
