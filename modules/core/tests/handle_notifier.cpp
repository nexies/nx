//
// Created by nexie on 24.03.2026.
//

#include <nx/core/App.hpp>

#include <nx/asio/context/reactor_handle.hpp>
#include <nx/asio/handle_notifier.hpp>
#include <termios.h>

namespace detail
{
    inline char hex_digit(unsigned v) {
        return static_cast<char>(v < 10 ? ('0' + v) : ('A' + (v - 10)));
    }

    inline void append_hex_byte(std::string& out, unsigned char ch) {
        out += '\\';
        out += 'x';
        out += hex_digit((ch >> 4) & 0x0F);
        out += hex_digit(ch & 0x0F);
    }

    inline bool is_csi_final(unsigned char ch) {
        return ch >= 0x40 && ch <= 0x7E;
    }

    inline bool is_intermediate(unsigned char ch) {
        return ch >= 0x20 && ch <= 0x2F;
    }

    inline bool is_parameter(unsigned char ch) {
        return ch >= 0x30 && ch <= 0x3F;
    }
}


inline std::string escape_ansi(std::string_view input) {
    std::string out;
    out.reserve(input.size() * 2);

    for (std::size_t i = 0; i < input.size();) {
        unsigned char ch = static_cast<unsigned char>(input[i]);

        // ANSI ESC sequence
        if (ch == 0x1B) {
            out += "\\e";

            if (i + 1 >= input.size()) {
                ++i;
                continue;
            }

            unsigned char next = static_cast<unsigned char>(input[i + 1]);

            // CSI: ESC [ ... final
            if (next == '[') {
                out += '[';
                i += 2;

                while (i < input.size()) {
                    unsigned char c = static_cast<unsigned char>(input[i]);
                    if (detail::is_parameter(c) || detail::is_intermediate(c)) {
                        out += static_cast<char>(c);
                        ++i;
                        continue;
                    }
                    if (detail::is_csi_final(c)) {
                        out += static_cast<char>(c);
                        ++i;
                        break;
                    }

                    // malformed sequence
                    detail::append_hex_byte(out, c);
                    ++i;
                    break;
                }
                continue;
            }

            // OSC: ESC ] ... BEL or ESC
            if (next == ']') {
                out += ']';
                i += 2;

                while (i < input.size()) {
                    unsigned char c = static_cast<unsigned char>(input[i]);

                    if (c == 0x07) { // BEL terminator
                        out += "\\a";
                        ++i;
                        break;
                    }

                    if (c == 0x1B && i + 1 < input.size()
                        && static_cast<unsigned char>(input[i + 1]) == '\\') {
                        out += "\\e\\";
                        i += 2;
                        break;
                    }

                    if (std::isprint(c)) {
                        out += static_cast<char>(c);
                    } else if (c == '\n') {
                        out += "\\n";
                    } else if (c == '\r') {
                        out += "\\r";
                    } else if (c == '\t') {
                        out += "\\t";
                    } else {
                        detail::append_hex_byte(out, c);
                    }
                    ++i;
                }
                continue;
            }

            // Two-byte or generic ESC sequence: ESC X
            out += static_cast<char>(next);
            i += 2;
            continue;
        }

        switch (ch) {
            case '\n': out += "\\n"; break;
            case '\r': out += "\\r"; break;
            case '\t': out += "\\t"; break;
            case '\b': out += "\\b"; break;
            case '\f': out += "\\f"; break;
            case '\v': out += "\\v"; break;
            case '\a': out += "\\a"; break;
            case '\\': out += "\\\\"; break;
            default:
                if (std::isprint(ch)) {
                    out += static_cast<char>(ch);
                } else {
                    detail::append_hex_byte(out, ch);
                }
                break;
        }

        ++i;
    }

    return out;
}

class terminal_raw_mode {
public:
    explicit terminal_raw_mode(int fd) : fd_(fd) {
        if (!isatty(fd_)) {
            throw std::runtime_error("fd is not a terminal");
        }

        if (tcgetattr(fd_, &old_) != 0) {
            throw std::runtime_error(std::string("tcgetattr failed: ") + std::strerror(errno));
        }

        termios raw = old_;

        raw.c_lflag &= ~(ICANON | ECHO);
        raw.c_cc[VMIN] = 1;   // read() вернёт, когда есть хотя бы 1 байт
        raw.c_cc[VTIME] = 0;  // без таймаута

        if (tcsetattr(fd_, TCSANOW, &raw) != 0) {
            throw std::runtime_error(std::string("tcsetattr failed: ") + std::strerror(errno));
        }

        active_ = true;
    }

    ~terminal_raw_mode() {
        restore();
    }

    terminal_raw_mode(const terminal_raw_mode&) = delete;
    terminal_raw_mode& operator=(const terminal_raw_mode&) = delete;

    void restore() {
        if (active_) {
            tcsetattr(fd_, TCSANOW, &old_);
            active_ = false;
        }
    }

private:
    int fd_;
    termios old_{};
    bool active_ = false;
};

void write_all(int fd, std::string_view s) {
    const char* p = s.data();
    std::size_t left = s.size();

    while (left > 0) {
        ssize_t n = ::write(fd, p, left);
        if (n <= 0) return;
        p += n;
        left -= static_cast<std::size_t>(n);
    }
}

void enable_mouse_tracking() {
    write_all(STDOUT_FILENO, "\x1b[?1003h"); // drag tracking
    write_all(STDOUT_FILENO, "\x1b[?1006h"); // SGR mode
}

void disable_mouse_tracking() {
    write_all(STDOUT_FILENO, "\x1b[?1006l");
    write_all(STDOUT_FILENO, "\x1b[?1003l");
}

class debug_notifier : public nx::asio::reactor_handle
{
    public:
    debug_notifier(nx::asio::io_context & ctx)
        : reactor_handle(ctx)
    { }

    void
    react(nx::asio::io_event event) override
    {
        switch (event)
        {
        case nx::asio::io_event::read:
            {
                char buf [128] { 0 };
                auto n = ::read(handle(), buf, sizeof(buf));
                nxDebug("[event read] fd:{}, bytes:{}, data:{}", handle(), n, escape_ansi(buf));
                break;
            }
        default:
            nxDebug("[event {}]", (int)event);
        }
    }
};

int main (int argc, char * argv [])
{
    using namespace nx;

    App::Init(argc, argv);

    // asio::handle_notifier notif(Thread::CurrentContext());
    terminal_raw_mode raw_t(fileno(stdin));
    enable_mouse_tracking();

    // dnotif.uninstall();

    debug_notifier dnotif(Thread::CurrentContext());
    dnotif.install(fileno(stdin), asio::io_interest::read);

    auto res =  App::Exec();
    disable_mouse_tracking();
    return 0;
}
