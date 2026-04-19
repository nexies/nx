//
// Created by nexie on 18.04.2026.
//

#ifndef NX_LOGGING_FORMATTED_SINK_HPP
#define NX_LOGGING_FORMATTED_SINK_HPP

#include <nx/logging/sinks/basic_sink.hpp>
#include <nx/logging/formatters/formatter.hpp>

namespace nx::logging {

    template <typename Mutex>
    class formatted_sink : public basic_sink {
    protected:
        formatter formatter_;
        Mutex mutex_;
    public:

        formatted_sink ()
            : basic_sink ()
            , formatter_ { default_formatter() }
        {}

        explicit
        formatted_sink (formatter formatter)
            : basic_sink ()
            , formatter_ (std::move(formatter))
        {}

        ~formatted_sink () override = default;

        NX_DISABLE_COPY(formatted_sink)
        NX_DISABLE_MOVE(formatted_sink)

    public:
        void
        log (const log_message & m) final {
            std::lock_guard<Mutex> lock (mutex_);
            memory_buffer_t buf;
            formatter_(m, buf);
            log_impl (buf);
        }

        void
        flush () final {
            std::lock_guard<Mutex> lock (mutex_);
            flush_impl();
        }

        virtual void
        set_pattern (const std::string & pattern) {
            std::lock_guard<Mutex> lock (mutex_);
            formatter_->set_pattern (pattern);
        }

        virtual void
        set_formatter (formatter_ptr formatter) {
            std::lock_guard<Mutex> lock (mutex_);
            formatter_ = std::move(formatter);
        }

    protected:
        virtual void
        log_impl (string_view_t string) = 0;

        virtual void
        flush_impl () = 0;
    };

}

#endif //NX_LOGGING_FORMATTED_SINK_HPP
