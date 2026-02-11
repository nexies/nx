//
// Created by nexie on 10.02.2026.
//

#include <nx/io/version.hpp>
#include "../../io/include/nx/io.hpp"

namespace nx::io
{
    std::string version_string()
    {
        return nx::io::version::str;
    }
}