//
// Created by nexie on 18.04.2026.
//

#ifndef NX_LOGGER_LEVELS_HPP
#define NX_LOGGER_LEVELS_HPP

namespace nx::logging {
    enum class level {
        critical = 1,
        error = 2,
        warning = 3,
        info = 4,
        debug = 5,
        trace = 6
    };

    static inline const char *
    level_string [] = {
        "critical",
        "error",
        "warning",
        "info",
        "debug",
        "trace"
    };

    static inline const char *
    level_string_upper [] = {
        "CRITICAL",
        "ERROR",
        "WARNING",
        "INFO",
        "DEBUG",
        "TRACE"
    };

    static inline const char *
    level_string_short [] = {
        "crit",
        "erro",
        "warn",
        "info",
        "debg",
        "trce"
    };
}

#endif //NX_LOGGER_LEVELS_HPP
