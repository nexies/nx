//
// Append-only file sink.
//

#ifndef NX_LOGGING_BASIC_FILE_SINK_HPP
#define NX_LOGGING_BASIC_FILE_SINK_HPP

#include <nx/logging/sinks/sink.hpp>

#include <cstdio>
#include <string>

namespace nx::logging {

class basic_file_sink final : public sink {
public:
    explicit
    basic_file_sink(std::string path)
        : path_ { std::move(path) }
    {
        file_ = std::fopen(path_.c_str(), "ab");
    }

    ~basic_file_sink() override
    {
        if (file_)
            std::fclose(file_);
    }

    basic_file_sink(const basic_file_sink&) = delete;
    basic_file_sink& operator=(const basic_file_sink&) = delete;

    [[nodiscard]] bool
    is_open() const noexcept
    {
        return file_ != nullptr;
    }

    void
    write(level msg_level, std::string_view formatted_line) override
    {
        if (!file_ || !should_log(msg_level))
            return;
        if (!formatted_line.empty())
            std::fwrite(formatted_line.data(), 1, formatted_line.size(), file_);
        std::fputc('\n', file_);
    }

    void
    flush() override
    {
        if (file_)
            std::fflush(file_);
    }

private:
    std::string path_;
    std::FILE* file_ { nullptr };
};

} // namespace nx::logging

#endif // NX_LOGGING_BASIC_FILE_SINK_HPP
