//
// Created by nexie on 18.04.2026.
//

#include <nx/logging/formatters/formatter.hpp>
#include <nx/common/format/fmt/bundled/color.h>


namespace nx::logging {

    formatter default_formatter() {
        return [] (const log_message& msg, memory_buffer_t & buf) -> void {
            fmt::format_to(std::back_inserter(buf),
            "[{}] {} \n",
            level_string[(int)msg.log_level],
            msg.payload);
        };
    }
}
