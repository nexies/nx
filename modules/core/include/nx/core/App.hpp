//
// Created by nexie on 09.11.2025.
//

#ifndef APP_HPP
#define APP_HPP

#include "nx/core.hpp"
#include <nx/core/object/Signal.hpp>
#include <nx/core/Object.hpp>
// #include "Dispatcher.hpp"

#include <filesystem>
#include <boost/program_options.hpp>
#include <boost/asio/signal_set.hpp>

#include <nx/core/app/Application.hpp>

namespace nx {
    using App = ::nx::core::Application;

# define nxApp (::nx::core::Application::Instance())
}



#endif //APP_HPP
