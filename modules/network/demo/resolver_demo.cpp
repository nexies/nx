// resolver_demo — resolves a few hostnames and prints the results.
//   Uses both async and sync paths to demonstrate the API.

#include <nx/network/network.hpp>
#include <nx/core2/core2.hpp>
#include <nx/logging.hpp>

int main()
{
    nx::logging::set_default_log_domain("demo_resolver");
    nx::core::local_thread main_thread("main");
    nx::core::loop         loop;

    using namespace nx::network;

    // ── Sync resolve (no thread needed) ──────────────────────────────────────

    nx_info("--- sync resolve ---");

    resolver::resolve_sync("localhost", 80)
        .and_then([](const std::vector<endpoint> & eps) -> nx::result<void> {
            for (const auto & ep : eps)
                nx_info("  localhost -> {}", ep.to_string());
            return {};
        })
        .or_else([](nx::error e) {
            nx_critical("sync resolve failed: {}", e.what());
        });

    // ── Async resolve ─────────────────────────────────────────────────────────

    nx_info("--- async resolve ---");

    resolver res;
    // Resolver lives on main_thread so signals are delivered here.
    res.move_to_thread(&main_thread);

    int pending = 0;

    auto on_done = [&]() {
        if (--pending == 0)
            loop.quit();
    };

    nx::core::connect(&res, &resolver::resolved, &res,
        [&](std::string host, std::vector<endpoint> eps) {
            for (const auto & ep : eps)
                nx_info("  {} -> {}", host, ep.to_string());
            on_done();
        });

    nx::core::connect(&res, &resolver::error_occurred, &res,
        [&](nx::error e) {
            nx_critical("async resolve failed: {}", e.what());
            on_done();
        });

    // Kick off two async lookups.
    auto submit = [&](std::string_view host, uint16_t port) {
        auto r = res.resolve(host, port);
        if (r.is_error()) {
            nx_critical("resolve() failed: {}", r.error().what());
            loop.exit(1);
            return;
        }
        ++pending;
        nx_info("resolving {}:{} ...", host, port);
    };

    submit("localhost", 443);
    submit("localhost", 8080);
    submit("google.com", 8080);
    submit("yandex.com", 8080);
    submit("nexie.ru", 8080);
    submit("youtube.com", 8080);

    return loop.exec();
}
