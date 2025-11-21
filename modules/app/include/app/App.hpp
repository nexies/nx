//
// Created by nexie on 09.11.2025.
//

#ifndef APP_HPP
#define APP_HPP

#include "nxapp.hpp"
#include "Signal.hpp"
#include "Object.hpp"
#include "app/Dispatcher.hpp"

#include <filesystem>
#include <boost/program_options.hpp>

namespace nx {
    using options_description = boost::program_options::options_description;

    class App : Object {
    public:
        static void Init (int argc, char * argv[]);
        static void Free ();

        //! Start application event-loop
        //! @return exit code
        static int Exec ();

        //! Exit the application (Gracefully)
        //! @param code exit code
        static void Exit (int code);

        //! Exit the application (Gracefully)
        //! @param res exit result (code + comment)
        static void Exit (const Result & res);

        //! Gracefully exit the application
        //! @param code exit code
        static void Quit ();

        //! Abort execution (hard-exit)
        static void Abort ();

        //! Set command-line application options
        //! @return
        static Result AddProgramOptions (const options_description & desc);
        static void SetApplicationName (const std::string & name);
        static std::string ApplicationName();

        // static Result Notify (Object *, Event *);

        template<typename Type>
        result_t<Type, const char *> GetParam (const std::string_view & name);
        template<typename Type>
        result_t<Type, const char *> GetEnv (const std::string_view & name);

    public: //TODO: signals
        static NX_SIGNAL(applicationNameChanged, const std::string &);
        static NX_SIGNAL(executionStart);
        static NX_SIGNAL(executionEnd);

    private:
        App();
        Result _init (int argc, char * argv[]);
        Result _makeMainThread ();
        Result _makeDispatcher ();
        Result _parseOptions (int argc, char * argv[]);
        Result _readDotEnvFile ();
        Result _createLogger (); // <- params ?
        Result _createEventLoop ();
        void _printAppInfo () const;
        Result _startEventLoop ();
        void _closeThreads ();

    private:
        static App * m_self;
        static App * _Self ();


        struct Preferences {
            using path = std::filesystem::path;
            std::string application_name {"nx_app"};
            path execution_path {"/"};
            options_description opt_desc;
            boost::program_options::variables_map options;
            path env_file {application_name + ".env"};
            path log_file {application_name + ".log"};
            spdlog::level::level_enum log_level =  spdlog::level::trace;
        } m_preferences;

        Dispatcher * m_dispatcher { nullptr };
    };
}



#endif //APP_HPP
