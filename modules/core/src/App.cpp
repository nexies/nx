//
// Created by nexie on 09.11.2025.
//

#include "nx/core.hpp"
#include <nx/core/version.hpp>
#include <nx/core/App.hpp>
#include <iostream>
#include <signal.h>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/ringbuffer_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/dist_sink.h>
#include <spdlog/common-inl.h>

#define MAIN_LOGGER_NAME "main"

#include <nx/core/Thread.hpp>
#include <nx/core/Loop.hpp>
#include <cstdlib>

#ifndef NX_TRACE_SIGNALS
#define NX_TRACE_SIGNALS 0
#define NX_TRACE_SIGNALS_LOGGER_NAME "signal"
#endif


// namespace log = spdlog;
namespace po = boost::program_options;
using namespace std;




/*
nx::App * nx::App::m_self { nullptr };

void nx::App::Init(int argc, char *argv[]) {
    auto self = _Self ();
    if (auto const res = self->_init(argc, argv); !res)
        Exit(res.get_err().code());

    // nxInfo("Application initialized [version {}, build at {}]", version(), build_time_utc());
}

void nx::App::Free() {
    if (m_self)
        delete m_self;
    m_self = nullptr;
}

int nx::App::Exec() {
    auto res = _Self()->_startEventLoop();
    if (!res) {
        nxCritical("Application error: {}", res.get_err().str());
        return res.get_err().code();
    }
    nxInfo("exiting with code {}", res.get_ok().code());
    return res.get_ok().code();
}

void nx::App::Exit(int code) {
    nxDevDebug("App::Exit called");
    auto self = _Self();

    NX_EMIT(self->aboutToQuit);
    NX_EMIT(self->_signalForExit, code);
}

void nx::App::Quit() {
    Exit(0);
}

void nx::App::Abort() {
    nxCritical("!!! ABORT !!!");
    ::std::abort();
}

// nx::TimerId nx::App::AddTimer(TimerType type, Duration dur, detail::timer_callback_t cb)
// {
//     // auto self = _Self();
//     // if (!self->m_dispatcher)
//         // return detail::invalid_timer;
//
//     // return self->m_dispatcher->addTimer(type, dur, std::move(cb));
//     return 0;
// }

// nx::Result nx::App::CancelTimer(TimerId timerId)
// {
// //    auto self = _Self();
// //     if (!self->m_dispatcher)
// //         return Result::Err("Dispatcher is not initialized");
// //     return self->m_dispatcher->cancelTimer(timerId);
//     return Result::Ok();
// }

nx::Result nx::App::AddProgramOptions(const options_description &desc) {
    _Self()->m_preferences.opt_desc.add(desc);
    return Result::Ok();
}

void nx::App::SetApplicationName(const std::string &name) {
    if (name != ApplicationName())
        _Self()->applicationNameChanged(name);
    _Self()->m_preferences.application_name = name;
}

std::string nx::App::ApplicationName() {
    return _Self()->m_preferences.application_name;
}

// nx::Result nx::App::Notify(Object* object, Event* event)
// {
//     return _Self()->notify(object, event);
// }

nx::App * nx::App::Get() {
    return _Self();
}

nx::App::App() :
    Object()
{

}

nx::Result nx::App::_init(int argc, char *argv[]) {
    auto res = _parseOptions(argc, argv);
    if (res) res = _makeMainThread();
    if (res) res = _readDotEnvFile();
    if (res) res = _createLogger();
    if (res) res = _createEventLoop();
    if (res) res = _makeDispatcher();
    if (res) _printAppInfo();
    return res;
}

nx::Result nx::App::_makeMainThread(){
    auto thread = Thread::FromCurrentThread();
    if (thread)
    {
        _reattachToThread(thread);
        nx::connect (this, &App::_signalForExit, this, &App::_exit, nx::Connection::Queued | nx::Connection::Unique);
        return Result::Ok();
    }


    //
    // sigset_t set;
    // sigemptyset(&set);
    // sigaddset(&set, SIGINT);
    // sigaddset(&set, SIGTERM);
    // sigaddset(&set, SIGQUIT);
    // pthread_sigmask(SIG_BLOCK, &set, nullptr);

    return Result::Err("Failed to create main thread");
}

nx::Result nx::App::_makeDispatcher() {

    auto & context = this->thread()->context();
    auto signals = new boost::asio::signal_set(thread()->context());

    signals->add(SIGINT);
    signals->add(SIGTERM);
    signals->add(SIGILL);
    signals->add(SIGABRT);
    signals->add(SIGFPE);
    signals->add(SIGSEGV);

    signals->add(SIGHUP);
    signals->add(SIGQUIT);
    signals->add(SIGTRAP);
    signals->add(SIGKILL);
    signals->add(SIGPIPE);
    signals->add(SIGALRM);


    signals->async_wait([this] (const boost::system::error_code & err, int signal)
    {
        if (!err) {
            this->_onSignalFromOS(signal);
        } else {
            std::cerr << "Signal wait error: " << err.message() << std::endl;
        }
    });

    return Result::Ok();
}

nx::Result nx::App::_parseOptions(int argc, char *argv[]) {
    m_preferences.executable =  argv[0];
    m_preferences.execution_path = std::filesystem::current_path();
    return Result::Ok();
}

nx::Result nx::App::_readDotEnvFile() {
    return Result::Ok();
}

nx::Result nx::App::_createLogger() {
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

    if (auto const file_sink = std::make_shared<basic_file_sink_mt>(m_preferences.log_file); file_sink)
    {
        combined->add_sink(file_sink);
        combined->set_level(spdlog::level::debug);
    }
    else
        return Result::Err("Failed to create file sink");

    combined->set_level(m_preferences.log_level);
    auto logger = std::make_shared<spdlog::logger>(MAIN_LOGGER_NAME, combined);
    logger->set_level(m_preferences.log_level);
    auto formatter = std::make_unique<spdlog::pattern_formatter>();
    formatter->add_flag<ThreadFormaterFlag>('T').set_pattern("[%Y-%m-%d %H:%M:%S.%f] [%n] ["/*%t|#1#"tid:%T] [%^%l%$] %v (%s:%#)");

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
        formatter->add_flag<ThreadFormaterFlag>('T').set_pattern("%^[%Y-%m-%d %H:%M:%S:%f] [%n] ["/*%t|#1#"tid:%T] [%l] %v %$(%s:%#)");
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
        formatter->add_flag<ThreadFormaterFlag>('T').set_pattern("[%Y-%m-%d %H:%M:%S:%f] [%n] ["/*%t|#1#"tid:%T] [%^%l%$] %v (%s:%#)");
        devel_logger->set_formatter(std::move(formatter));

        spdlog::register_logger(devel_logger);
    }
#endif
    return Result::Ok();
}

nx::Result nx::App::_createEventLoop() {
    return Result::Ok();
}

void nx::App::_printAppInfo() const {
    nxDevInfo("Application '{}'", m_preferences.application_name);
    nxDevInfo("   --- powered by nx::app version {}",  nx::core::version::str);
    nxDevInfo("   --- build date: {}", nx::core::version::build_time_utc);
    nxDevInfo("   --- log level: {}", spdlog::level::to_string_view(m_preferences.log_level));
    nxDevInfo("   --- executable: {}", m_preferences.executable.string());
    nxDevInfo("   --- exec path: {}", m_preferences.execution_path.string());
}

nx::Result nx::App::_startEventLoop() {
    Loop loop;
    return loop.exec();
}

void nx::App::_closeThreads()
{
    detail::ThreadInfo::Instance().exitAllThreads();
    detail::ThreadInfo::Instance().waitForAllThreadsExit();
}

void nx::App::_exit(int code)
{
    auto self = _Self();
    nxDevTrace("App::_exit");
    self->_closeThreads();
}

void nx::App::_onSignalFromOS(int signal)
{
    switch (signal)
    {
    case SIGINT:
    case SIGTERM:
    case SIGHUP:
    case SIGQUIT:
        ::nx::App::Quit();
        break;
    default:
        NX_EMIT(this->signalFromOS, signal)
        break;
    }
}


nx::App * nx::App::_Self() {
    if (!m_self)
        m_self = new App();
    return m_self;
}
*/
