#pragma once

#include <nx/common/platform/standard_defs.hpp>

// ── nx::filesystem ────────────────────────────────────────────────────────────
// C++17+: alias to std::filesystem.
// C++11/14: minimal path stub — only exposes filename() and string() needed by
//           nx::source_location.  Extend as required.

#if defined(NX_CPP17) || defined(NX_CPP20) || defined(NX_CPP23)

#include <filesystem>
namespace nx {
    namespace filesystem = std::filesystem;
}

#else

#include <string>
#include <cstring>

namespace nx {
namespace filesystem {

class path {
public:
    path() = default;
    path(const char * p)        : p_(p ? p : "") {}
    path(const std::string & p) : p_(p)           {}

    std::string string() const { return p_; }

    path filename() const {
        const std::size_t sl = p_.find_last_of("/\\");
        return (sl == std::string::npos) ? path(p_) : path(p_.substr(sl + 1));
    }

    bool empty() const noexcept { return p_.empty(); }

private:
    std::string p_;
};

} // namespace filesystem
} // namespace nx

#endif // NX_CPP17+
