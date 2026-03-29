//
// Created by nexie on 23.03.2026.
//

#include <nx/common/platform.hpp>

#include "signal_set_impl.hpp"
#include "signal_set_posix.inl"
#include "signal_set_kevent.inl"

namespace nx::asio {
    std::shared_ptr<signal_set::impl>
    signal_set::impl::create_impl(io_context &ctx) {
# if defined(NX_OS_LINUX)
        return std::make_shared<signal_set_posix>(ctx);
#elif defined(NX_OS_APPLE)
        return std::make_shared<signal_set_kevent>(ctx);
#elif defined (NX_OS_WINDOWS)
        return nullptr;
#else
# error "signal_set is not supported. please reconfigure"
#endif
        return nullptr;
    }
}