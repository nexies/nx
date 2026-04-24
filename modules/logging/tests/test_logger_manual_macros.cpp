//
// Manual check: logging via NX_LOG_LOGGER_* macros — output must include source file + line
// (pattern %s / %#). NO_CATCH, same style as logger_manual.cpp.
//

#include <nx/logging/macros.hpp>
#include <nx/logging/pattern_formatter.hpp>
#include <nx/logging/registry.hpp>
#include <nx/logging/logger.hpp>
#include <nx/logging/sinks/sink.hpp>

#include <cctype>
#include <cstdlib>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

using namespace nx::logging;

namespace {

int
fail(char const* what)
{
    std::cerr << "[logger_manual_macros] FAIL: " << what << '\n';
    return 1;
}

struct capture_sink final : sink {
    std::vector<std::string> lines;

    void
    write(level msg_level, std::string_view formatted_line) override
    {
        if (!should_log(msg_level))
            return;
        lines.emplace_back(formatted_line.data(), formatted_line.size());
    }

    void
    flush() override {}
};

[[nodiscard]] bool
has_cpp_line_ref(std::string_view line)
{
    auto const pos = line.find(".cpp:");
    if (pos == std::string_view::npos)
        return false;
    std::size_t i = pos + 5;
    if (i >= line.size())
        return false;
    return std::isdigit(static_cast<unsigned char>(line[i])) != 0;
}

[[nodiscard]] bool
contains_this_file(std::string_view line)
{
    return line.find("logger_manual_macros.cpp") != std::string_view::npos;
}

} // namespace

// Separate functions so macro line numbers are stable and distinct.
static void
emit_info_a(std::shared_ptr<logger> const& lg)
{
    NX_LOG_LOGGER_INFO(lg, "macro message A");
}

static void
emit_warn_b(std::shared_ptr<logger> const& lg)
{
    NX_LOG_LOGGER_WARN(lg, "macro message B ({})", 42);
}

int
main()
{
    auto cap = std::make_shared<capture_sink>();
    cap->set_level(level::trace);

    auto lg = std::make_shared<logger>("macros_demo", cap);
    lg->set_level(level::trace);

    auto fmt = std::make_unique<pattern_formatter>();
    // %s = basename, %# = line — filled from nx::source_location::current() inside macros
    fmt->set_pattern("[%L] %v  (%s:%#)");
    lg->set_formatter(std::move(fmt));

    set_default_logger(lg);

    std::cout << "--- Macros: NX_LOG_LOGGER_* with source_location (also captured below) ---\n";

    emit_info_a(lg);
    emit_warn_b(lg);

    NX_LOG_LOGGER_DEBUG(lg, "via default logger getter: {}", 3);
    NX_LOG_LOGGER_TRACE(get_default_logger(), "trace with registry logger");

    set_default_logger(nullptr);
    lg->flush();

    if (cap->lines.size() < 4)
        return fail("expected at least 4 log lines from macros");

    for (std::size_t i = 0; i < cap->lines.size(); ++i) {
        std::string const& line = cap->lines[i];
        if (!contains_this_file(line))
            return fail("line missing this translation unit file name");
        if (!has_cpp_line_ref(line))
            return fail("line missing .cpp:<line> source location");
    }

    std::cout << "\n--- Captured lines (pattern [%L] %v  (%s:%#)) ---\n";
    for (auto const& line : cap->lines)
        std::cout << line << '\n';

    std::cout << "\n[logger_manual_macros] OK\n";
    return 0;
}
