//
// Global logging registry.
//

#include <nx/logging/registry.hpp>

#include <mutex>
#include <unordered_map>

namespace nx::logging {

namespace {

std::mutex g_mutex;
std::shared_ptr<logger> g_default;
std::unordered_map<std::string, std::weak_ptr<logger>> g_named;

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
get(std::string const& name)
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
