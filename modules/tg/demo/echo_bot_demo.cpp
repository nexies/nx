// echo_bot_demo — Telegram echo bot built on nx::tg.
//
// The bot replies to every text message with the same text.
//
// Usage:
//   echo_bot_demo <token>
//   echo_bot_demo          (reads TELEGRAM_BOT_TOKEN from environment)

#include <nx/tg/tg.hpp>
#include <nx/core2/core2.hpp>
#include <nx/logging.hpp>

#include <cstdlib>
#include <string>

int main(int argc, char * argv[])
{
    nx::logging::set_default_log_domain("echo_bot");

    std::string token;
    if (argc >= 2) {
        token = argv[1];
    } else if (const char * env = std::getenv("TELEGRAM_BOT_TOKEN")) {
        token = env;
    }

    if (token.empty()) {
        nx_critical("Usage: echo_bot_demo <token>  or  set TELEGRAM_BOT_TOKEN");
        return 1;
    }

    nx::core::local_thread main_thread("main");
    nx::core::loop         loop;

    nx::tg::bot bot(token);

    nx::core::connect(&bot, &nx::tg::bot::message_received, &bot,
        [&bot](const nx::tg::message & msg) {
            if (msg.text.empty()) return;
            nx_info("[{}] {}: {}", msg.chat.id, msg.from.first_name, msg.text);
            bot.send_message(msg.chat.id, msg.text);
        });

    nx::core::connect(&bot, &nx::tg::bot::error_occurred, &bot,
        [](nx::error e) { nx_warning("bot error: {}", e.what()); });

    bot.start();
    nx_info("Echo bot started. Send a message in Telegram, Ctrl+C to stop.");

    return loop.exec();
}
