//
// Manual / integration checks for nx::logging (no Catch2). Exercises loggers, sinks,
// patterns, registry, and prints throughput numbers.
//
// Environment (optional, positive integers):
//   NX_LOGGING_BENCH_DISCARD — iterations for discard-sink benchmark (default 12000).
//   NX_LOGGING_BENCH_FILE    — iterations for file-sink benchmark (default 8000).
// Use a Release build and larger values for serious throughput measurement.
//

#include <nx/logging/logging.hpp>

#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <string_view>

namespace fs = std::filesystem;
using namespace nx::logging;

namespace {

int
fail(char const* what)
{
    std::cerr << "[logger_manual] FAIL: " << what << '\n';
    return 1;
}

struct discard_sink final : sink {
    void
    write(level /*msg_level*/, std::string_view /*formatted_line*/) override
    {}

    void
    flush() override {}
};

int
env_positive_int(char const* name, int default_value)
{
    char const* const s = std::getenv(name);
    if (!s || !*s)
        return default_value;
    char* end = nullptr;
    long const v = std::strtol(s, &end, 10);
    if (end == s || v <= 0)
        return default_value;
    if (v > 10000000L)
        return 10000000;
    return static_cast<int>(v);
}

bool
file_contains(fs::path const& path, std::string_view needle)
{
    std::ifstream in(path);
    if (!in)
        return false;
    std::string const content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    return content.find(needle) != std::string::npos;
}

void
thread_id_flag(log_message const&, memory_buffer_t& buf)
{
    static constexpr char k[] = "tid";
    buf.append(k, k + sizeof(k) - 1);
}

} // namespace

int
main()
{
    std::error_code ec;

    // --- Temp log files ---
    fs::path const path_combined = fs::temp_directory_path() / "nx_logging_manual_combined.log";
    fs::path const path_app2 = fs::temp_directory_path() / "nx_logging_manual_app2.log";
    fs::remove(path_combined, ec);
    fs::remove(path_app2, ec);

    // --- Logger A: dist(console + file), custom pattern with %T ---
    auto file_a = std::make_shared<basic_file_sink>(path_combined.string());
    if (!file_a->is_open())
        return fail("basic_file_sink (combined) did not open");

    auto console = std::make_shared<stdout_sink>();
    console->set_level(level::trace);
    console->set_color_mode(color_mode::always);
    file_a->set_level(level::info);

    auto dist = std::make_shared<dist_sink>();
    dist->set_level(level::trace);
    dist->add_sink(console);
    dist->add_sink(file_a);

    auto main_lg = std::make_shared<logger>("manual", dist);
    main_lg->set_level(level::trace);

    auto fmt_main = std::make_unique<pattern_formatter>();
    fmt_main->add_custom_flag('T', thread_id_flag);
    fmt_main->set_pattern("[%Y-%m-%d %H:%M:%S] [%n] [tid:%T] [%^%L%$] %v");
    main_lg->set_formatter(std::move(fmt_main));

    std::cout << "--- Logger \"manual\" (console + file), pattern with date + custom T ---\n";
    main_lg->log(level::info, "combined sink: hello console and file");
    main_lg->log(level::warn, "second line (level warn)");
    main_lg->flush();
    file_a->flush();

    if (!file_contains(path_combined, "combined sink"))
        return fail("combined log file missing expected text");
    if (!file_contains(path_combined, "manual"))
        return fail("combined log file missing logger name");

    // --- Registry: register + get ---
    register_logger(main_lg);
    if (get("manual") != main_lg)
        return fail("registry get(\"manual\")");
    set_default_logger(main_lg);
    if (get_default_logger() != main_lg)
        return fail("get_default_logger");

    // --- Logger B: file only, minimal pattern ---
    auto file_b = std::make_shared<basic_file_sink>(path_app2.string());
    if (!file_b->is_open())
        return fail("basic_file_sink (app2) did not open");

    auto app2 = std::make_shared<logger>("app2", file_b);
    app2->set_level(level::trace);
    auto fmt_b = std::make_unique<pattern_formatter>();
    fmt_b->set_pattern("%n | %l | %v");
    app2->set_formatter(std::move(fmt_b));

    register_logger(app2);
    app2->log(level::warn, "file-only logger");
    app2->flush();

    if (!file_contains(path_app2, "app2"))
        return fail("app2 file missing logger name");
    if (!file_contains(path_app2, "file-only"))
        return fail("app2 file missing message");

    // --- Throughput: no I/O (discard sink) ---
    // Override with e.g. NX_LOGGING_BENCH_DISCARD=500000 for a longer run (prefer Release).
    int const k_discard_iters = env_positive_int("NX_LOGGING_BENCH_DISCARD", 12000);
    auto discard = std::make_shared<discard_sink>();
    discard->set_level(level::trace);
    auto bench_lg = std::make_shared<logger>("bench", discard);
    bench_lg->set_level(level::trace);
    auto fmt_bench = std::make_unique<pattern_formatter>();
    fmt_bench->set_pattern("[%H:%M:%S] [%n] [%L] %v");
    bench_lg->set_formatter(std::move(fmt_bench));

    using clock = std::chrono::steady_clock;
    auto const t0 = clock::now();
    for (int i = 0; i < k_discard_iters; ++i)
        bench_lg->log(level::debug, "iter {}", i);
    auto const t1 = clock::now();
    auto const us = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
    double const sec = us / 1e6;
    double const lines_per_sec = sec > 0.0 ? static_cast<double>(k_discard_iters) / sec : 0.0;

    std::cout << "\n--- Benchmark: discard sink (format + dispatch, no I/O) ---\n";
    std::cout << "  iterations: " << k_discard_iters << "\n";
    std::cout << "  time:       " << us << " us\n";
    std::cout << "  throughput: " << std::fixed << std::setprecision(0) << lines_per_sec << " lines/s\n";

    // --- Throughput: file append (realistic I/O) ---
    fs::path const path_bench = fs::temp_directory_path() / "nx_logging_manual_bench_io.log";
    fs::remove(path_bench, ec);
    int const k_file_iters = env_positive_int("NX_LOGGING_BENCH_FILE", 8000);
    auto file_io = std::make_shared<basic_file_sink>(path_bench.string());
    if (!file_io->is_open())
        return fail("bench file sink open");

    auto io_lg = std::make_shared<logger>("bench_io", file_io);
    io_lg->set_level(level::trace);
    auto fmt_io = std::make_unique<pattern_formatter>();
    fmt_io->set_pattern("%n %v");
    io_lg->set_formatter(std::move(fmt_io));

    auto const t2 = clock::now();
    for (int i = 0; i < k_file_iters; ++i)
        io_lg->log(level::info, "line {}", i);
    io_lg->flush();
    file_io->flush();
    auto const t3 = clock::now();
    auto const us_io = std::chrono::duration_cast<std::chrono::microseconds>(t3 - t2).count();
    double const sec_io = us_io / 1e6;
    double const lines_file = sec_io > 0.0 ? static_cast<double>(k_file_iters) / sec_io : 0.0;

    std::cout << "\n--- Benchmark: basic_file_sink (append + flush at end) ---\n";
    std::cout << "  iterations: " << k_file_iters << "\n";
    std::cout << "  time:       " << us_io << " us\n";
    std::cout << "  throughput: " << std::fixed << std::setprecision(0) << lines_file << " lines/s\n";
    std::cout << "  file:       " << path_bench << '\n';

    // Cleanup temp files
    fs::remove(path_combined, ec);
    fs::remove(path_app2, ec);
    fs::remove(path_bench, ec);

    set_default_logger(nullptr);
    std::cout << "\n[logger_manual] OK\n";
    return 0;
}
