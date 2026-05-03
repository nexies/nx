//
// connection_info — implementation
//

#include <nx/core2/object/connection_info.hpp>

#include <algorithm>

namespace nx::core {

// ──────────────────────────────────────────────────────────────────────────────
// connection_info
// ──────────────────────────────────────────────────────────────────────────────

bool
connection_info::add_connection(detail::connection_entry entry)
{
    std::lock_guard lock { mutex_ };

    if (entry.is_unique()) {
        auto it = by_sender_.find(entry.sender_key);
        if (it != by_sender_.end()) {
            for (const auto & e : it->second)
                if (e.receiver == entry.receiver && e.slot_id == entry.slot_id)
                    return false;
        }
    }

    by_sender_[entry.sender_key].push_back(std::move(entry));
    return true;
}

bool
connection_info::remove_connection(detail::connection_id_t id)
{
    std::lock_guard lock { mutex_ };

    for (auto & [sk, list] : by_sender_) {
        auto it = std::find_if(list.begin(), list.end(),
                               [id](const auto & e) { return e.id == id; });
        if (it != list.end()) {
            list.erase(it);
            return true;
        }
    }
    return false;
}

bool
connection_info::remove_connection_by_key(detail::sender_key_t sender_key,
                                           void * receiver,
                                           detail::function_id_t slot_id)
{
    std::lock_guard lock { mutex_ };

    auto it = by_sender_.find(sender_key);
    if (it == by_sender_.end())
        return false;

    auto & list = it->second;
    auto entry_it = std::find_if(list.begin(), list.end(),
                                 [receiver, slot_id](const auto & e) {
                                     return e.receiver == receiver &&
                                            e.slot_id  == slot_id;
                                 });
    if (entry_it == list.end())
        return false;

    list.erase(entry_it);
    return true;
}

int
connection_info::remove_connections_by_key_and_receiver(
    detail::sender_key_t sender_key, void * receiver)
{
    std::lock_guard lock { mutex_ };

    auto it = by_sender_.find(sender_key);
    if (it == by_sender_.end())
        return 0;

    auto & list = it->second;
    int count = 0;
    auto entry_it = list.begin();
    while (entry_it != list.end()) {
        if (entry_it->receiver == receiver) {
            entry_it = list.erase(entry_it);
            ++count;
        } else {
            ++entry_it;
        }
    }
    return count;
}

bool
connection_info::has_connection(detail::connection_id_t id) const
{
    std::lock_guard lock { mutex_ };

    for (const auto & [sk, list] : by_sender_)
        for (const auto & e : list)
            if (e.id == id)
                return true;
    return false;
}

std::vector<detail::connection_entry>
connection_info::connections_for(detail::sender_key_t sender_key) const
{
    std::lock_guard lock { mutex_ };

    auto it = by_sender_.find(sender_key);
    if (it == by_sender_.end())
        return {};
    return it->second; // snapshot copy
}

} // namespace nx::core
