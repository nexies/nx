//
// Created by nexie on 04.03.2026.
//

#include <iostream>
#include <nx/core/app/Application.hpp>
#include <nx/core/Thread.hpp>

#include <nx/core/Loop.hpp>
#include <nx/core/detail/logger_defs.hpp>

#include "spdlog/pattern_formatter.h"
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/ringbuffer_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/dist_sink.h>
#include <spdlog/details/log_msg.h>
#include <spdlog/common-inl.h>

// #include <boost/asio/signal_set.hpp>
#include <nx/asio/signal_set.hpp>

// boost::asio::signal_set * g_signal_set;
nx::asio::signal_set * g_signal_set;

namespace
{
    class ThreadFormaterFlag : public spdlog::custom_flag_formatter
    {
    public:
        void format(const spdlog::details::log_msg &, const std::tm &, spdlog::memory_buf_t &dest) override
        {
            std::string txt = "n/a";
            auto id = ::nx::Thread::CurrentId();
            if (id != ::nx::detail::g_invalidThreadId)
            {
                txt = std::to_string(id);
            }
            dest.append(txt.data(), txt.data() + txt.size());
        }

        std::unique_ptr<custom_flag_formatter> clone() const override
        {
            return spdlog::details::make_unique<ThreadFormaterFlag>();
        }
    };
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
        using namespace spdlog::sinks;
    auto combined = std::make_shared<spdlog::sinks::dist_sink_mt>();

    if (auto const console_sink = std::make_shared<stdout_color_sink_mt>(); console_sink)
    {
        console_sink->set_color_mode(spdlog::color_mode::always);
        console_sink->set_level(spdlog::level::trace);
        combined->add_sink(console_sink);
    }
    else
        return Result::Err("Failed to create console log sink");

    if (auto const trace_sink = std::make_shared<stdout_color_sink_mt>(); trace_sink)
    {
        trace_sink->set_color_mode(spdlog::color_mode::always);
    }

    // if (auto const file_sink = std::make_shared<basic_file_sink_mt>(m_preferences.log_file); file_sink)
    // {
    //     combined->add_sink(file_sink);
    //     combined->set_level(spdlog::level::debug);
    // }
    // else
    //     return Result::Err("Failed to create file sink");

    combined->set_level(spdlog::level::trace);
    auto logger = std::make_shared<spdlog::logger>("nx", combined);
    logger->set_level(spdlog::level::trace);
    auto formatter = std::make_unique<spdlog::pattern_formatter>();
    formatter->add_flag<ThreadFormaterFlag>('T').set_pattern("[%Y-%m-%d %H:%M:%S.%f] [%n] ["/*%t|#1#*/"tid:%T] [%^%l%$] %v (%s:%#)");

    logger->set_formatter(std::move(formatter));
    spdlog::set_default_logger(logger);

#if NX_TRACE_SIGNALS
    {
        auto const console_sink = std::make_shared<stdout_color_sink_mt>();
        console_sink->set_color_mode(spdlog::color_mode::always);
        console_sink->set_level(spdlog::level::trace);
        auto signal_logger = std::make_shared<spdlog::logger>(NX_TRACE_SIGNALS_LOGGER_NAME, console_sink);
        signal_logger->set_level(spdlog::level::trace);

        formatter = std::make_unique<spdlog::pattern_formatter>();
        formatter->add_flag<ThreadFormaterFlag>('T').set_pattern("%^[%Y-%m-%d %H:%M:%S:%f] [%n] ["/*%t|#1#*/"tid:%T] [%l] %v %$(%s:%#)");
        signal_logger->set_formatter(std::move(formatter));

        spdlog::register_logger(signal_logger);
    }
#endif

#if NX_DEVEL_LOGGING
    {
        auto const console_sink = std::make_shared<stdout_color_sink_mt>();
        console_sink->set_color_mode(spdlog::color_mode::always);
        console_sink->set_level(spdlog::level::trace);
        auto devel_logger = std::make_shared<spdlog::logger>(NX_DEVEL_LOGGER_NAME, console_sink);
        devel_logger->set_level(spdlog::level::trace);

        formatter = std::make_unique<spdlog::pattern_formatter>();
        formatter->add_flag<ThreadFormaterFlag>('T').set_pattern("[%Y-%m-%d %H:%M:%S:%f] [%n] ["/*%t|#1#*/"tid:%T] [%^%l%$] %v (%s:%#)");
        devel_logger->set_formatter(std::move(formatter));

        spdlog::register_logger(devel_logger);
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
