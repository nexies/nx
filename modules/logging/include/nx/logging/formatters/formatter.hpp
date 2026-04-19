//
// Created by nexie on 18.04.2026.
//

#ifndef NX_LOGGING_FORMATTER_HPP
#define NX_LOGGING_FORMATTER_HPP

#include <nx/logging/message.hpp>
#include <nx/common.hpp>


namespace nx::logging {
    // class formatter {
    //     std::string pattern_;
    // public:
    //     virtual
    //     ~formatter() = default;
    //
    //     virtual error
    //     format(const log_message & message, memory_buffer_t & buffer) = 0;
    // };
    //
    // using
    // formatter_ptr = std::unique_ptr<formatter>;
    //
    // formatter_ptr
    // clone_formatter (formatter_ptr formatter);
    //
    // formatter_ptr
    // make_default_formatter ();
    //
    // formatter_ptr
    // make_formatter (const std::string & pattern);

    using formatter = std::function<void(const log_message&, memory_buffer_t &)>;

    formatter default_formatter();
}

#endif //NX_LOGGING_FORMATTER_HPP
