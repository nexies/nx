//
// Created by nexie on 09.11.2025.
//

#include "app/App.hpp"
#include <iostream>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/ringbuffer_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

// namespace log = spdlog;
namespace po = boost::program_options;
using namespace std;
nx::Application * nx::Application::m_self { nullptr };

void nx::Application::Init(int argc, char *argv[]) {
    auto self = _Self ();
    if (auto const res = self->init(argc, argv); !res)
        Exit(res);

    nxInfo("Application initialized");
}

void nx::Application::Free() {
    if (m_self)
        delete m_self;
    m_self = nullptr;
}

int nx::Application::Exec() {
    auto res = _Self()->start_event_loop();
    if (!res) {
        nxCritical("Application error: {}", res.get_err().str());
        return res.get_err().code();
    }
    return res.get_ok().code();
}

void nx::Application::Exit(int code) {
    if (code)
        std::cerr << "Exiting with code " << code << "..." << std::endl;
    else
        std::cout << "Exiting with code " << code << "..." << std::endl;
    std::exit(code);
}

void nx::Application::Exit(const Result & res) {
    if (!res) { // <- is error
        std::cerr << "Exiting with error: " << res.get_err().str() << " (code:" << res.get_err().code() << ")" << std::endl;
        std::exit(res.get_err().code());
    }
    else {
        std::cout << "Exiting with success: " << res.get_ok().str() << " (code:" << res.get_ok().code() << ")" << std::endl;
        std::exit(res.get_ok().code());
    }
}

void nx::Application::Quit(int code) {
}

void nx::Application::Quit(const Result &res) {
}

void nx::Application::Abort() {
    std::abort();
}

nx::Result nx::Application::AddProgramOptions(const options_description &desc) {
    _Self()->m_preferences.opt_desc.add(desc);
    return Result::Ok();
}

void nx::Application::SetApplicationName(const std::string &name) {
    if (name != ApplicationName())
        nx::emit_signal(applicationNameChanged, name);
    _Self()->m_preferences.application_name = name;
}

std::string nx::Application::ApplicationName() {
    return _Self()->m_preferences.application_name;
}

nx::Application::Application() {
}

nx::Result nx::Application::init(int argc, char *argv[]) {
    auto res = parse_options(argc, argv);
    if (res) res = read_dot_env_file();
    if (res) res = create_logger();
    if (res) res = create_event_loop();
    return res;
}

nx::Result nx::Application::parse_options(int argc, char *argv[]) {
    return Result::Ok();
}

nx::Result nx::Application::read_dot_env_file() {
    return Result::Ok();
}

#include <spdlog/sinks/dist_sink.h>
#include <spdlog/common-inl.h>
#define MAIN_LOGGER_NAME "main"
nx::Result nx::Application::create_logger() {
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
    spdlog::set_default_logger(logger);
    nxInfo("Application logger installed. Log level=\"{}\"", spdlog::level::level_string_views[m_preferences.log_level]);
    return Result::Ok();
}

nx::Result nx::Application::create_event_loop() {
    return Result::Ok();
}

nx::Result nx::Application::start_event_loop() {
    return Result::Err("Event loop is not yet implemented");
    nx::emit_signal(executionStart);
}

nx::Application * nx::Application::_Self() {
    if (!m_self)
        m_self = new Application();
    return m_self;
}
