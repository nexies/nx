//
// Created by nexie on 04.03.2026.
//

#include <iostream>
#include <nx/core/app/Application.hpp>
#include <nx/core/Thread.hpp>

#include <nx/core/Loop.hpp>
#include <nx/core/detail/logger_defs.hpp>

#include <nx/logging/logging.hpp>
#include <nx/logging/message.hpp>
#include <nx/logging/pattern_formatter.hpp>
#include <nx/logging/types.hpp>

#include <string>

// #include <boost/asio/signal_set.hpp>
#include <nx/asio/signal_set.hpp>

// boost::asio::signal_set * g_signal_set;
nx::asio::signal_set * g_signal_set;

namespace
{
    void
    thread_id_flag(nx::logging::log_message const&, nx::logging::memory_buffer_t& dest)
    {
        std::string txt = "n/a";
        auto const id = ::nx::Thread::CurrentId();
        if (id != ::nx::detail::g_invalidThreadId)
        {
            txt = std::to_string(id);
        }
        dest.append(txt.data(), txt.data() + txt.size());
    }
}

namespace nx::core
{
    Application * Application::s_instance { nullptr };

    Application::Application() :
        Object()
    {
        if (s_instance) {
            nxCritical("Application created twice");
            throw nx::Exception("Application created twice");
        }

        s_instance = this;

        _createLogger();
        _makeMainThread();
        _beginAsyncWaitSIGNAL();
    }

    Application::Application(int argc, char* argv[]) :
        Application ()
    {
        _parseArguments (argc, argv);
    }

    int Application::exec() {
        auto res = _startEventLoop();
        // _removeMainThread();
        if (!res) {
            nxDebug("Exiting with error code {}: \"\"", res.get_err().code());
            return res.get_err().code();
        } else {
            nxDebug("Exiting with code {}", res.get_ok().code());
            return res.get_ok().code();
        }
    }

    void Application::quit() {
        exit(0);
    }

    void Application::exit(int code) {
        // std::cerr << "Emmiting _signalForExit" << std::endl;
        NX_EMIT(_signalForExit, code);
    }

    void Application::abort(){
        std::abort();
    }

    Result Application::init() {
        return init(0, nullptr);
    }

    Result Application::init(int argc, char *argv[]) {
        _parseArguments(argc, argv);
        _makeMainThread();
        _beginAsyncWaitSIGNAL();
        return Result::Ok();
    }


    Result Application::_beforeExec() {
        // nxTrace("Applicaiton::_beforeExec");
        return Result::Ok();
    }

    Result Application::_afterExec() {
        // nxTrace("Application::_afterExec");
        return Result::Ok();
    }

    void Application::_onSIGNAL(int signal) {
        switch (signal) {
        default:
            nxDebug("Received OS signal: {}", signal);
            NX_EMIT(signalFromOS, signal)
            break;
        case SIGINT:
        case SIGQUIT:
        case SIGTERM:
        case SIGPIPE:
            // std::cerr << "Received exit signal" << std::endl;
            quit();
            break;
        }
    }

    void Application::_printAppInfo() const {

    }

    Result Application::_parseArguments(int args, char* argv[]) {
        return Result::Ok();
    }

    Result Application::_createLogger() {
        auto combined = std::make_shared<nx::logging::dist_sink>();

        auto const console_sink = std::make_shared<nx::logging::stdout_sink>();
        console_sink->set_color_mode(nx::logging::color_mode::always);
        console_sink->set_level(nx::logging::level::trace);
        combined->add_sink(console_sink);

        combined->set_level(nx::logging::level::trace);
        auto main_logger = std::make_shared<nx::logging::logger>("nx", combined);
        main_logger->set_level(nx::logging::level::trace);

        auto formatter = std::make_unique<nx::logging::pattern_formatter>();
        formatter->add_custom_flag('T', thread_id_flag);
        formatter->set_pattern("[%Y-%m-%d %H:%M:%S.%f] [%n] [tid:%T] [%^%l%$] %v (%s:%#)");

        main_logger->set_formatter(std::move(formatter));
        nx::logging::set_default_logger(main_logger);

#if NX_TRACE_SIGNALS
        {
            auto const signal_console = std::make_shared<nx::logging::stdout_sink>();
            signal_console->set_color_mode(nx::logging::color_mode::always);
            signal_console->set_level(nx::logging::level::trace);
            auto signal_logger = std::make_shared<nx::logging::logger>(NX_TRACE_SIGNALS_LOGGER_NAME, signal_console);
            signal_logger->set_level(nx::logging::level::trace);

            auto fmt2 = std::make_unique<nx::logging::pattern_formatter>();
            fmt2->add_custom_flag('T', thread_id_flag);
            fmt2->set_pattern("%^[%Y-%m-%d %H:%M:%S:%f] [%n] [tid:%T] [%l] %v %$(%s:%#)");
            signal_logger->set_formatter(std::move(fmt2));

            nx::logging::register_logger(signal_logger);
        }
#endif

#if NX_DEVEL_LOGGING
        {
            auto const devel_console = std::make_shared<nx::logging::stdout_sink>();
            devel_console->set_color_mode(nx::logging::color_mode::always);
            devel_console->set_level(nx::logging::level::trace);
            auto devel_logger = std::make_shared<nx::logging::logger>(NX_DEVEL_LOGGER_NAME, devel_console);
            devel_logger->set_level(nx::logging::level::trace);

            auto fmt3 = std::make_unique<nx::logging::pattern_formatter>();
            fmt3->add_custom_flag('T', thread_id_flag);
            fmt3->set_pattern("[%Y-%m-%d %H:%M:%S.%f] [%n] [tid:%T] [%^%l%$] %v (%s:%#)");
            devel_logger->set_formatter(std::move(fmt3));

            nx::logging::register_logger(devel_logger);
        }
#endif
        return Result::Ok();
    }

    Result Application::_makeMainThread() {
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

    Result Application::_beginAsyncWaitSIGNAL()
    {
        if (!thread())
            return Result::Err("Application::_asyncWaitSIGNAL: Thread is not initialized");

        // g_signal_set = new boost::asio::signal_set(thread()->context());
        g_signal_set = new nx::asio::signal_set(Thread::CurrentContext());

        g_signal_set->add(SIGINT);
        g_signal_set->add(SIGTERM);
        g_signal_set->add(SIGILL);
        // signals->add(SIGABRT);
        // signals->add(SIGFPE);
        // signals->add(SIGSEGV);

        g_signal_set->add(SIGHUP);
        g_signal_set->add(SIGQUIT);
        // signals->add(SIGTRAP);
        g_signal_set->add(SIGPIPE);
        g_signal_set->add(SIGWINCH);
        // signals->add(SIGALRM);

        g_signal_set->async_wait(_asyncWaitSIGNAL);
        return Result::Ok();
    }

    Result Application::_startEventLoop() {
        Loop loop;
        _beforeExec();
        auto res = loop.exec();
        _afterExec();
        return res;
    }

    void Application::_asyncWaitSIGNAL(result<int> signal_result)
    {
        if (!signal_result)
        {
            nxError("Error in _asyncWaitSignal: {}", signal_result.error().description());
            return;
        }
        if (!s_instance)
        {
            nxCritical("Error in _asyncWaitSignal: Application is not initialized");
            return;
        }
        s_instance->_onSIGNAL(signal_result.value());
    }

    // void Application::_asyncWaitSIGNAL(/*const boost::system::error_code & er,*/ int signal_code)
    // {
    //     if (s_instance)
    //         s_instance->_onSIGNAL(signal_code);
    //     else
    //         nxCritical("Error in asyncWaitSIGNAL: Application is not initialized");
    //
    //     g_signal_set->async_wait(_asyncWaitSIGNAL);
    // }

    void Application::_closeThreads(int exit_code) {
        // std::cerr << "_closeThreads" << std::endl;
        detail::ThreadInfo::Instance().exitAllThreads(exit_code);
        detail::ThreadInfo::Instance().waitForAllThreadsExit();
    }

    void Application::_doExit(int code) {
        // std::cerr << "_DoExit" << std::endl;
        _closeThreads(code);
    }

    void Application::_removeMainThread() {
        if (thread())
            delete thread();
    }

    void Application::Init(int argc, char *argv[]) {
        new Application(argc, argv);
    }

    void Application::Init() {
        new Application();
    }

    int Application::Exec() {
        return s_instance->exec();
    }

    void Application::Quit() {
        s_instance->quit();
    }

    void Application::Exit(int exit_code) {
        s_instance->exit(exit_code);
    }

    void Application::Abort() {
        s_instance->abort();
    }

    Application * Application::Instance() {
        return s_instance;
    }
}
