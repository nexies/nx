//
// Created by nexie on 09.11.2025.
//

#ifndef APP_HPP
#define APP_HPP

#include "nxapp.hpp"
#include "Signal.hpp"
#include "Object.hpp"

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

        static Result Notify (Object *, Event *);

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
        Result init (int argc, char * argv[]);
        Result make_main_thread ();
        Result parse_options (int argc, char * argv[]);
        Result read_dot_env_file ();
        Result create_logger (); // <- params ?
        Result create_event_loop ();
        Result start_event_loop ();

        Result onTimer(TimerEvent *) override;
        Result onEvent(Event*) override;

        void exit_impl () const;

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
            spdlog::level::level_enum log_level;
        } m_preferences;

        Thread * main_thread { nullptr };
    };
}



#endif //APP_HPP
