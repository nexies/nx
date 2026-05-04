//
// Global logging registry.
//

#include <nx/logging/registry.hpp>

#include <mutex>
#include <unordered_map>
#include <nx/logging/sinks/stdout_sink.hpp>

namespace nx::logging {

namespace {

std::mutex g_mutex;
std::shared_ptr<logger> g_default;
std::unordered_map<std::string, std::weak_ptr<logger>> g_named;

    void create_default_logger()
    {
        auto console = std::make_shared<stdout_sink>();
        console->set_level(level::trace);

        auto logger = std::make_shared<nx::logging::logger>("nx", console);
        logger->set_level(level::trace);

        auto fmt = std::make_unique<pattern_formatter>();
        fmt->set_pattern("[%Y-%m-%d %H:%M:%S.%f] [%n] [%L] %v (%s:%#)");
        logger->set_formatter(std::move(fmt));
        // set_default_logger(std::move(logger));
        g_default = std::move(logger);
    }

} // namespace

void
set_default_logger(std::shared_ptr<logger> lg)
{
    std::lock_guard<std::mutex> lock { g_mutex };
    g_default = std::move(lg);
}

std::shared_ptr<logger>
get_default_logger()
{
    std::lock_guard<std::mutex> lock { g_mutex };
    if (!g_default)
        create_default_logger();

    return g_default;
}

void
register_logger(std::shared_ptr<logger> lg)
{
    if (!lg)
        return;
    std::lock_guard<std::mutex> lock { g_mutex };
    g_named[lg->name()] = lg;
}

std::shared_ptr<logger>
get_logger(std::string const& name)
{
    std::lock_guard<std::mutex> lock { g_mutex };
    auto it = g_named.find(name);
    if (it == g_named.end())
        return nullptr;
    auto p = it->second.lock();
    if (!p)
        g_named.erase(it);
    return p;
}

} // namespace nx::logging
