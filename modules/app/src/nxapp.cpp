//
// Created by nexie on 14.11.2025.
//

#include <nx/app.hpp>
#include <nx/app/version.hpp>

namespace nx {
    std::string version() {
        return ::nx::app::version::str;
    }

    std::string build_time_utc() {
        return ::nx::app::version::build_time_utc;
    }
}
