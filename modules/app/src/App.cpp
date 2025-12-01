//
// Created by nexie on 09.11.2025.
//

#include "nx/app.hpp"
#include "nx/app/App.hpp"
#include <iostream>
#include <signal.h>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/ringbuffer_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/dist_sink.h>
#include <spdlog/common-inl.h>

#define MAIN_LOGGER_NAME "main"

#include "nx/app/Thread.hpp"

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
    nxDebug("App::Exit called");
    nxTrace("App::Exit(), code {}", code);
    auto self = _Self();

    /*emit*/ self->aboutToQuit();

    // self->_generateSignal(Signal::Custom(self, &App::_exit, code), 0);
    nxTrace("emit signal for exit", code);
    self->_signalForExit(code);
}

void nx::App::Quit() {
    Exit(0);
}

void nx::App::Abort() {
    std::abort();
}

nx::TimerId nx::App::AddTimer(TimerType type, Duration dur, detail::timer_callback_t cb)
{
    // auto self = _Self();
    // if (!self->m_dispatcher)
        // return detail::invalid_timer;

    // return self->m_dispatcher->addTimer(type, dur, std::move(cb));
    return 0;
}

nx::Result nx::App::CancelTimer(TimerId timerId)
{
//    auto self = _Self();
//     if (!self->m_dispatcher)
//         return Result::Err("Dispatcher is not initialized");
//     return self->m_dispatcher->cancelTimer(timerId);
    return Result::Ok();
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
    auto signal = new boost::asio::signal_set(thread()->context(), SIGINT, SIGTERM);

    signal->async_wait([] (const boost::system::error_code & err, int signal)
    {
        if (!err) {
            // A signal occurred.
            std::cout << "Received signal: " << signal << std::endl;
            // Perform cleanup or graceful shutdown here.
            nx::App::Quit();
        } else {
            // An error occurred during signal wait.
            std::cerr << "Signal wait error: " << err.message() << std::endl;
        }
    });

    // m_signal = boost::asio::signal_set (thread()->context(), SIGINT, SIGTERM);
    // m_poll_thread = new PollThread();
    // m_poll_thread->addService(std::make_shared<SystemSignalPollService>());
    // m_poll_thread->start();

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
    if (m_preferences.log_level == spdlog::level::trace)
        formatter->add_flag<ThreadFormaterFlag>('T').set_pattern("[%Y-%m-%d %H:%M:%S] [%n] ["/*%t|*/"tid:%T] [%^%l%$] %v (%!)");
    else
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

    // PollLoop loop;
    // loop.setWaitDuration(Milliseconds(1));
    // loop.addService(std::make_shared<SystemSignalPollService>());
    // return loop.exec();

    Loop loop;
    return loop.exec();
}

void nx::App::_closeThreads()
{
    nxTrace("Closing all threads ({})", detail::ThreadInfo::Instance().threadCount());
    detail::ThreadInfo::Instance().exitAllThreads();
    detail::ThreadInfo::Instance().waitForAllThreadsExit();
}

void nx::App::_exit(int code)
{
    auto self = _Self();
    nxTrace("App::_exit");
    nxTrace("App attached thread id {}", self->threadId());
    self->_closeThreads();
}

nx::App * nx::App::_Self() {
    if (!m_self)
        m_self = new App();
    return m_self;
}
