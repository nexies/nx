//
// Created by nexie on 18.04.2026.
//

#include <nx/logging/message.hpp>
#include <thread>

nx::logging::log_message::log_message()
    : logger_name {  }
    , log_level { level::trace }
    , location { g_undefined_location }
    , time_point {  }
    , tid { 0 }
{

}

nx::logging::log_message::log_message(
    string_view_t logger_name,
    string_view_t payload,
    level_t log_level,
    source_location_t location,
    time_point_t time_point,
    tid_t tid)
    : logger_name{logger_name}
    , payload{payload}
    , log_level{log_level}
    , location{location}
    , time_point{time_point}
    , tid{tid}
{}

nx::logging::log_message::log_message(
    string_view_t logger_name,
    string_view_t payload,
    level_t log_level,
    source_location_t location)
    : logger_name{logger_name}
    , payload{payload}
    , log_level{log_level}
    , location{location}
    , time_point{clock_t::now()}
    , tid{0} {

}
