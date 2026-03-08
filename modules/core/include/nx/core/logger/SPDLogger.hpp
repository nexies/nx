//
// Created by nexie on 08.03.2026.
//

#ifndef NX_CORE_SPDLOGGER_HPP
#define NX_CORE_SPDLOGGER_HPP

#include <nx/core/types/Singleton.hpp>

namespace nx {
    class SPDLoggerInstance {
    public:
        SPDLoggerInstance();

        template<typename... Args>
        void trace(std::string_view category, Args... args);
        template<typename... Args>
        void debug(std::string_view category, Args... args);
        template<typename... Args>
        void warning(std::string_view category, Args... args);
        template<typename... Args>
        void error(std::string_view category, Args... args);
        template<typename... Args>
        void critical(std::string_view category, Args... args);
    };

    template<typename ... Args>
    void SPDLoggerInstance::trace(std::string_view category, Args... args) {

    }

    template<typename ... Args>
    void SPDLoggerInstance::debug(std::string_view category, Args... args) {
    }

    template<typename ... Args>
    void SPDLoggerInstance::warning(std::string_view category, Args... args) {
    }

    template<typename ... Args>
    void SPDLoggerInstance::error(std::string_view category, Args... args) {
    }

    template<typename ... Args>
    void SPDLoggerInstance::critical(std::string_view category, Args... args) {
    }

    using Logger = Singleton<SPDLoggerInstance>;
}

#endif //SPDLOGGER_HPP
