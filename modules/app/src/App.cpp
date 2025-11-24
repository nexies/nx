//
// Created by nexie on 09.11.2025.
//

#include "app/App.hpp"
#include <iostream>
#include <signal.h>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/ringbuffer_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/dist_sink.h>
#include <spdlog/common-inl.h>

#define MAIN_LOGGER_NAME "main"

#include "app/Loop.hpp"
#include "app/Thread.hpp"

// namespace log = spdlog;
namespace po = boost::program_options;
using namespace std;

namespace
{
#include "spdlog/pattern_formatter.h"
    class ThreadFormaterFlag : public spdlog::custom_flag_formatter
    {
    public:
        void format(const spdlog::details::log_msg &, const std::tm &, spdlog::memory_buf_t &dest) override
        {
            std::string txt = "n/a";
            auto id = ::nx::Thread::currentId();
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
    nxDebug("App::Exit()");
    auto self = _Self();

    /*emit*/ self->aboutToQuit();

    self->_generateSignal(Signal::Custom(self, &App::_exit, code), 0);
}

// void nx::App::Exit(const Result & res) {
//     if (!res) { // <- is error
//         std::cerr << "Exiting with error: " << res.get_err().str() << " (code:" << res.get_err().code() << ")" << std::endl;
//         std::exit(res.get_err().code());
//     }
//     else {
//         std::cout << "Exiting with success: " << res.get_ok().str() << " (code:" << res.get_ok().code() << ")" << std::endl;
//         std::exit(res.get_ok().code());
//     }
// }

void nx::App::Quit() {
    Exit(0);
}

void nx::App::Abort() {
    std::abort();
}

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
    auto thread = Thread::fromCurrentThread();
    if (thread)
    {
        _reattachToThread(thread);
        return Result::Ok();
    }


    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGTERM);
    sigaddset(&set, SIGQUIT);
    pthread_sigmask(SIG_BLOCK, &set, nullptr);

    return Result::Err("Failed to create main thread");
}

nx::Result nx::App::_makeDispatcher() {
    m_dispatcher = new MainDispatcher;
    m_dispatcher->start();
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

    // if (auto const trace_sink = std::make_shared<stdout_color_sink_mt>(); trace_sink)
    // {
    //     trace_sink->set_color_mode(spdlog::color_mode::always);
    // }

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
    formatter->add_flag<ThreadFormaterFlag>('T').set_pattern("[%Y-%m-%d %H:%M:%S] [%n] ["/*%t|*/"tid:%T] [%^%l%$] %v (%s:%#)");
    logger->set_formatter(std::move(formatter));
    // logger->set_pattern("[%Y-%m-%d %H:%M:%S] [%n] [%t] [%^%l%$] %v (%s:%#)");
    spdlog::set_default_logger(logger);
    // nxInfo("Application logger installed. Log level=\"{}\"", spdlog::level::to_string_view(m_preferences.log_level));
    return Result::Ok();
}

nx::Result nx::App::_createEventLoop() {
    return Result::Ok();
}

void nx::App::_printAppInfo() const {
    nxInfo("Application '{}'", m_preferences.application_name);
    nxInfo("   --- powered by nx::app version {}",  version());
    nxInfo("   --- build date: {}", build_time_utc());
    nxInfo("   --- log level: {}", spdlog::level::to_string_view(m_preferences.log_level));
    nxInfo("   --- executable: {}", m_preferences.executable.string());
    nxInfo("   --- exec path: {}", m_preferences.execution_path.string());
}

nx::Result nx::App::_startEventLoop() {
    Loop loop;
    return loop.exec();
}

// nx::Result nx::App::onTimer(TimerEvent* timer_event)
// {
//     return Object::onTimer(timer_event);
// }
//
// nx::Result nx::App::onEvent(Event* event)
// {
//     if (!event)
//         return Result::Err("Bad nullptr event");
//
//     switch (event->type())
//     {
//         case Event::Type::Exit: _closeThreads(); return Result::Ok();
//     }
//     return Object::onEvent(event);
// }

void nx::App::_closeThreads()
{
    nxDebug("Closing all threads");
    detail::ThreadInfo::Instance().exitAllThreads();
    detail::ThreadInfo::Instance().waitForAllThreadsExit();
}

void nx::App::_exit(int code)
{
    auto self = _Self();
    self->_closeThreads();
    // self->_generateSignal(Signal::Exit(self->_getLocalThread()->loop(), code), 10);
}

nx::App * nx::App::_Self() {
    if (!m_self)
        m_self = new App();
    return m_self;
}
