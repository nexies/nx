#pragma once
#include <nx/common/tweaks.hpp>

#if defined(NX_USE_STD_FORMAT)
# include <format>
#endif

#if defined(NX_USE_EXTERNAL_FMT)
# include <fmt/ranges.h>
#endif

#if defined(NX_USE_BUNDLED_FMT)
# include "bundled/ranges.h"
#endif
