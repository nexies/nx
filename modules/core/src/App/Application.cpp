//
// Created by nexie on 04.03.2026.
//

#include <nx/core/App/Application.hpp>

#include <nx/core/Thread.hpp>

namespace nx
{
    Application::Application() :
        Object()
    {

    }

    Application::Application(int argc, char* argv[]) :
        Application ()
    {
        _parseArguments (argc, argv);
    }

    Result Application::_parseArguments(int args, char* argv[])
    {
        return Result::Ok();
    }

    Result Application::_makeMainThread()
    {
        auto main_thread = Thread::FromCurrentThread();
        if (main_thread)
        {
            _reattachToThread(main_thread);
            ::nx::connect(this, &Application::_signalForExit, this, &Application::_doExit,
                ::nx::Connection::Unique | ::nx::Connection::Queued);
            return Result::Ok();
        }

        return Result::Err("Failure while creating main thread");
    }

    Result Application::_asyncWaitSIGNAL()
    {
    }

    Result Application::_doExit()
    {
    }

}
