#pragma once

#include <cstdint>
#include <optional>
#include <string>

namespace nx::tg {

struct user {
    int64_t     id         { 0 };
    bool        is_bot     { false };
    std::string first_name;
    std::string last_name;
    std::string username;
};

struct chat {
    int64_t     id { 0 };
    std::string type;   // "private", "group", "supergroup", "channel"
    std::string title;
    std::string username;
    std::string first_name;
    std::string last_name;
};

struct message {
    int64_t     message_id { 0 };
    user        from;
    tg::chat    chat;
    int64_t     date       { 0 };
    std::string text;
};

struct update {
    int64_t                update_id { 0 };
    std::optional<message> message;
};

} // namespace nx::tg
