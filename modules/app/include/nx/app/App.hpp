//
// Created by nexie on 09.11.2025.
//

#ifndef APP_HPP
#define APP_HPP

#include "nx/app.hpp"
#include "Signal.hpp"
#include "nx/app/Object.hpp"
#include "Dispatcher.hpp"

#include <filesystem>
#include <boost/program_options.hpp>
#include <boost/asio/signal_set.hpp>

namespace nx {
    using options_description = boost::program_options::options_description;

    class App : public Object {
    public:
        static void Init (int argc, char * argv[]);
        static void Free ();

        //! Start application event-loop
        //! @return exit code
        static int Exec ();

        //! Exit the application (Gracefully)
        //! @param code exit code
        static void Exit (int code);

        //! Gracefully exit the application
        //! @param code exit code
        static void Quit ();

        //! Abort execution (hard-exit)
        static void Abort ();

        static TimerId AddTimer(TimerType, Duration, detail::timer_callback_t);

        static Result CancelTimer (TimerId timerId);

        //! Set command-line application options
        //! @return
        static Result AddProgramOptions (const options_description & desc);

        //! Application name setter
        static void SetApplicationName (const std::string & name);
        //! Application name getter
        static std::string ApplicationName();

        template<typename Type>
        result_t<Type, const char *> GetParam (const std::string_view & name);
        template<typename Type>
        result_t<Type, const char *> GetEnv (const std::string_view & name);

        NX_SIGNAL(applicationNameChanged, const std::string &);
        NX_SIGNAL(aboutToQuit)

        static App * Get ();
    private:
        App();
        Result _init (int argc, char * argv[]);
        Result _parseOptions (int argc, char * argv[]);
        Result _readDotEnvFile ();
        Result _createLogger (); // <- params ?
        Result _makeMainThread ();
        Result _makeDispatcher ();
        Result _createEventLoop ();
        void _printAppInfo () const;
        Result _startEventLoop ();
        void _closeThreads ();

        NX_SIGNAL(_signalForExit, int)
        void _exit(int code);

    private:
        static App * m_self;
        static App * _Self ();


        struct Preferences {
            using path = std::filesystem::path;
            std::string application_name {"nx_app"};
            path execution_path {"/"};
            path executable {"/"};
            options_description opt_desc;
            boost::program_options::variables_map options;
            path env_file {application_name + ".env"};
            path log_file {application_name + ".log"};
            // spdlog::level::level_enum log_level = spdlog::level::trace;
            spdlog::level::level_enum log_level =  spdlog::level::debug;
        } m_preferences;

        // MainDispatcher * m_dispatcher { nullptr };
        PollThread * m_poll_thread { nullptr };
        // boost::asio::signal_set m_signal;
    };
}



#endif //APP_HPP
