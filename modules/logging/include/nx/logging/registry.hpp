//
// Global default logger and named loggers (spdlog-style registry).
//

#ifndef NX_LOGGING_REGISTRY_HPP
#define NX_LOGGING_REGISTRY_HPP

#include <nx/logging/logger.hpp>

#include <memory>
#include <string>

namespace nx::logging {

void
set_default_logger(std::shared_ptr<logger> lg);

[[nodiscard]] std::shared_ptr<logger>
get_default_logger();

void
register_logger(std::shared_ptr<logger> lg);

[[nodiscard]] std::shared_ptr<logger>
get(std::string const& name);

} // namespace nx::logging

#endif // NX_LOGGING_REGISTRY_HPP
