//
// application — implementation of nx::core::application
//

#include <nx/core2/app/application.hpp>
#include <nx/core2/loop/loop.hpp>

#include <nx/core2/detail/logger_defs.hpp>

#include <nx/logging/logging.hpp>
#include <nx/logging/message.hpp>
#include <nx/logging/pattern_formatter.hpp>
#include <nx/logging/sinks/stdout_sink.hpp>
#include <nx/logging/sinks/dist_sink.hpp>
#include <nx/logging/registry.hpp>

#include <csignal>
#include <iostream>
#include <stdexcept>

namespace nx::core {

application * application::instance_ = nullptr;

// ── Logging ───────────────────────────────────────────────────────────────────

namespace {

void
thread_id_flag(nx::logging::log_message const & /*msg*/,
               nx::logging::memory_buffer_t & dest)
{
    const auto id = thread::current_id();
    const auto s  = std::to_string(id);
    dest.append(s.data(), s.data() + s.size());
}

} // anonymous namespace

// ──────────────────────────────────────────────────────────────────────────────
// application
// ──────────────────────────────────────────────────────────────────────────────

application::application()
    : object(nullptr)
{
    if (instance_)
        throw std::runtime_error("nx::core::application created twice");
    instance_ = this;

    if (auto res = _init(0, nullptr); !res)
        throw std::runtime_error(res.error().description());
}

application::application(int argc, char * argv[])
    : object(nullptr)
{
    if (instance_)
        throw std::runtime_error("nx::core::application created twice");
    instance_ = this;

    if (auto res = _init(argc, argv); !res)
        throw std::runtime_error(res.error().description());
}

application::~application()
{
    instance_ = nullptr;
}

// ── Init chain ────────────────────────────────────────────────────────────────

nx::result<void>
application::_init(int /*argc*/, char * /*argv*/[])
{
    if (auto r = _create_logger();       !r) return r;
    if (auto r = _create_main_thread();  !r) return r;
    if (auto r = _setup_os_signals();    !r) return r;

    connect_queued(this, &application::_do_exit, this, &application::_handle_exit);
    return {};
}

nx::result<void>
application::_create_logger()
{
    using namespace nx::logging;

    auto combined = std::make_shared<dist_sink>();

    auto console = std::make_shared<stdout_sink>();
    console->set_level(level::trace);
    combined->add_sink(std::move(console));
    combined->set_level(level::trace);

    auto logger = std::make_shared<nx::logging::logger>("nx", combined);
    logger->set_level(level::trace);

    auto fmt = std::make_unique<pattern_formatter>();
    fmt->add_custom_flag('T', thread_id_flag);
    fmt->set_pattern("[%Y-%m-%d %H:%M:%S.%f] [%n] [tid:%T] [%^%l%$] %v (%s:%#)");
    logger->set_formatter(std::move(fmt));

    set_default_logger(std::move(logger));
    return {};
}

nx::result<void>
application::_create_main_thread()
{
    main_thread_ = std::make_unique<local_thread>("main");
    // Move the application object to live on the main thread
    move_to_thread(main_thread_.get());
    return {};
}

nx::result<void>
application::_setup_os_signals()
{
    signal_set_ = std::make_unique<nx::asio::signal_set>(
        thread::current_context());

    signal_set_->add(SIGINT);
    signal_set_->add(SIGTERM);
    signal_set_->add(SIGHUP);
    signal_set_->add(SIGQUIT);
    signal_set_->add(SIGPIPE);

    signal_set_->async_wait(&application::_os_signal_handler);
    return {};
}

// ── Exec ──────────────────────────────────────────────────────────────────────

int
application::exec()
{
    _print_startup_info();
    running_ = true;
    loop l;
    const int code = l.exec();
    running_ = false;
    return code;
}

// ── Control ───────────────────────────────────────────────────────────────────

void
application::quit()
{
    exit(0);
}

void
application::exit(int code)
{
    NX_EMIT(_do_exit, code);
}

bool
application::is_running() const noexcept
{
    return running_;
}

void
application::_handle_exit(int code)
{
    NX_EMIT(about_to_quit);
    detail::thread_registry::instance().exit_all(code);
    detail::thread_registry::instance().wait_all();
    if (auto * l = loop::current())
        l->exit(code);
}

// ── Info ──────────────────────────────────────────────────────────────────────

const std::string &
application::app_name() const noexcept
{
    return app_name_;
}

void
application::set_app_name(const std::string & name)
{
    app_name_ = name;
}

// ── Signals ───────────────────────────────────────────────────────────────────

void
application::_on_os_signal(int signum)
{
    nxDebug("Received OS signal: {}", signum);
    NX_EMIT(os_signal_received, signum);

    switch (signum) {
    case SIGINT:
    case SIGTERM:
    case SIGQUIT:
    case SIGHUP:
        quit();
        break;
    default:
        break;
    }
}

void
application::_os_signal_handler(nx::result<int> sig_result)
{
    if (!instance_)
        return;
    if (!sig_result) {
        nxError("OS signal error: {}", sig_result.error().description());
        return;
    }
    instance_->_on_os_signal(sig_result.value());
}

void
application::_print_startup_info() const
{
    nxInfo("application '{}' starting", app_name_);
}

application *
application::instance() noexcept
{
    return instance_;
}

} // namespace nx::core
