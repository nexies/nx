//
// Created by nexie on 03.03.2026.
//

#ifndef NX_APP_HPP
#define NX_APP_HPP

#include <nx/core/Object.hpp>
#include <nx/core/types/Singleton.hpp>

namespace nx::core
{
    class Application : public Object
    {
        // struct Preferences {
        //     using path = std::filesystem::path;
        //     std::string application_name {"nx_app"};
        //     path execution_path {"/"};
        //     path executable {"/"};
        //     options_description opt_desc;
        //     boost::program_options::variables_map options;
        //     path env_file {application_name + ".env"};
        //     path log_file {application_name + ".log"};
        //     spdlog::level::level_enum log_level = spdlog::level::trace;
        //     // spdlog::level::level_enum log_level =  spdlog::level::debug;
        // } m_preferences;

    public:
        Application ();
        Application (int argc, char * argv[]);

        int exec ();
        void quit ();
        void exit (int code);
        void abort ();

        Result init ();
        virtual Result init (int argc, char * argv[]);

        NX_SIGNAL(aboutToQuit)
        NX_SIGNAL(signalFromOS, int)

    protected:
        virtual Result _beforeExec ();
        virtual Result _afterExec ();

        virtual void _onSIGNAL(int signal);
        virtual void _printAppInfo() const;

        NX_SIGNAL(_signalForExit, int)
    private:
        Result _parseArguments (int args, char * argv[]);
        Result _createLogger ();
        Result _makeMainThread ();
        Result _asyncWaitSIGNAL ();
        Result _startEventLoop ();
        void _closeThreads (int exit_code);
        void _doExit (int code);
        void _removeMainThread ();

    /// STATIC FUNCTIONS
    public:
        static void Init(int argc, char * argv[]);

        static void Init();

        static int  Exec();

        static void Quit();

        static void Exit(int exit_code);

        static void Abort();

    /// STATIC MEMBER
    private:
        static Application * s_instance;
    };
}


#endif //NX_APP_HPP