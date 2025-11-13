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
nx::App * nx::App::m_self { nullptr };

void nx::App::Init(int argc, char *argv[]) {
    auto self = _Self ();
    if (auto const res = self->init(argc, argv); !res)
        Exit(res);

    nxInfo("Application initialized");
}

void nx::App::Free() {
    if (m_self)
        delete m_self;
    m_self = nullptr;
}

int nx::App::Exec() {
    auto res = _Self()->start_event_loop();
    if (!res) {
        nxCritical("Application error: {}", res.get_err().str());
        return res.get_err().code();
    }
    Free();
    detail::ThreadInfo::Free();
    return res.get_ok().code();
}

void nx::App::Exit(int code) {
    // _Self()->exit_impl();
    // if (code)
    //     std::cerr << "Exiting with code " << code << "..." << std::endl;
    // else
    //     std::cout << "Exiting with code " << code << "..." << std::endl;
    // std::exit(code);

    _Self()->_generateEvent(_Self(), new Event(Event::Exit), 10);
}

void nx::App::Exit(const Result & res) {
    if (!res) { // <- is error
        std::cerr << "Exiting with error: " << res.get_err().str() << " (code:" << res.get_err().code() << ")" << std::endl;
        std::exit(res.get_err().code());
    }
    else {
        std::cout << "Exiting with success: " << res.get_ok().str() << " (code:" << res.get_ok().code() << ")" << std::endl;
        std::exit(res.get_ok().code());
    }
}

void nx::App::Quit() {
    _Self()->_getLocalThread()->quit();
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
        nx::emit_signal(applicationNameChanged, name);
    _Self()->m_preferences.application_name = name;
}

std::string nx::App::ApplicationName() {
    return _Self()->m_preferences.application_name;
}

nx::Result nx::App::Notify(Object* object, Event* event)
{
    return _Self()->notify(object, event);
}

nx::App::App() :
    Object()
{

}

nx::Result nx::App::init(int argc, char *argv[]) {
    auto res = parse_options(argc, argv);
    if (res) res = make_main_thread();
    if (res) res = read_dot_env_file();
    if (res) res = create_logger();
    if (res) res = create_event_loop();
    return res;
}

nx::Result nx::App::make_main_thread(){
    auto thread = Thread::fromCurrentThread();
    if (thread)
    {
        _reattachToThread(thread);
        return Result::Ok();
    }
    return Result::Err("Failed to create main thread");
}

nx::Result nx::App::parse_options(int argc, char *argv[]) {
    return Result::Ok();
}

nx::Result nx::App::read_dot_env_file() {
    return Result::Ok();
}

nx::Result nx::App::create_logger() {
    using namespace spdlog::sinks;
    auto combined = std::make_shared<spdlog::sinks::dist_sink_mt>();

    if (auto const console_sink = std::make_shared<stdout_color_sink_mt>(); console_sink)
        combined->add_sink(console_sink);
    else
        return Result::Err("Failed to create console log sink");

    if (auto const file_sink = std::make_shared<basic_file_sink_mt>(m_preferences.log_file); file_sink)
        combined->add_sink(file_sink);
    else
        return Result::Err("Failed to create file sink");

    combined->set_level(m_preferences.log_level);
    auto logger = std::make_shared<spdlog::logger>(MAIN_LOGGER_NAME, combined);
    logger->set_level(m_preferences.log_level);
    logger->set_pattern("[%Y-%m-%d %H:%M:%S] [%n] [%^%l%$] %v (%s:%#)");
    spdlog::set_default_logger(logger);
    nxInfo("Application logger installed. Log level=\"{}\"", spdlog::level::level_string_views[m_preferences.log_level]);
    return Result::Ok();
}

nx::Result nx::App::create_event_loop() {
    return Result::Ok();
}

nx::Result nx::App::start_event_loop() {
    Loop loop;
    return loop.exec();
}

nx::Result nx::App::onTimer(TimerEvent* timer_event)
{
    return Object::onTimer(timer_event);
}

nx::Result nx::App::onEvent(Event* event)
{
    if (!event)
        return Result::Err("Bad nullptr event");

    switch (event->type())
    {
        case Event::Type::Exit: exit_impl(); return Result::Ok();
    }
    return Object::onEvent(event);
}

void nx::App::exit_impl() const
{
    detail::ThreadInfo::Instance().exitAllThreads();
    detail::ThreadInfo::Instance().waitForAllThreadsExit();
    if (main_thread) delete main_thread;
}

nx::App * nx::App::_Self() {
    if (!m_self)
        m_self = new App();
    return m_self;
}
