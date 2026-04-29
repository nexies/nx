//
// connection_info — implementation
//

#include <nx/core2/object/connection_info.hpp>
#include <nx/core2/object/object.hpp>

#include <algorithm>

namespace nx::core {

// ──────────────────────────────────────────────────────────────────────────────
// connection_info
// ──────────────────────────────────────────────────────────────────────────────

connection_info::connection_info(object * owner)
    : owner_(owner)
{}

connection_info::~connection_info() = default;

// ── Outgoing ──────────────────────────────────────────────────────────────────

bool
connection_info::add_connection(detail::connection_entry entry)
{
    std::lock_guard lock { mutex_ };

    // Unique check: compare by (receiver, slot_id) since IDs are now counters
    if (entry.is_unique()) {
        auto it = by_sender_.find(entry.sender_key);
        if (it != by_sender_.end()) {
            for (const auto & e : it->second)
                if (e.receiver == entry.receiver && e.slot_id == entry.slot_id)
                    return false;
        }
    }

    const auto id   = entry.id;
    const auto sk   = entry.sender_key;
    void * recv_ptr = entry.receiver;

    by_sender_[sk].push_back(std::move(entry));

    if (recv_ptr)
        by_receiver_[recv_ptr].insert(id);

    return true;
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

    const auto id = entry_it->id;
    list.erase(entry_it);

    if (receiver) {
        auto rit = by_receiver_.find(receiver);
        if (rit != by_receiver_.end()) {
            rit->second.erase(id);
            if (rit->second.empty())
                by_receiver_.erase(rit);
        }
    }
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
            void * recv = it->receiver;
            list.erase(it);

            if (recv) {
                auto rit = by_receiver_.find(recv);
                if (rit != by_receiver_.end()) {
                    rit->second.erase(id);
                    if (rit->second.empty())
                        by_receiver_.erase(rit);
                }
            }
            return true;
        }
    }
    return false;
}

void
connection_info::remove_connections_to(void * receiver)
{
    std::lock_guard lock { mutex_ };

    auto rit = by_receiver_.find(receiver);
    if (rit == by_receiver_.end())
        return;

    const auto ids = rit->second; // copy
    by_receiver_.erase(rit);

    for (const auto id : ids) {
        for (auto & [sk, list] : by_sender_) {
            auto it = std::find_if(list.begin(), list.end(),
                                   [id](const auto & e) { return e.id == id; });
            if (it != list.end()) {
                list.erase(it);
                break;
            }
        }
    }
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
            const auto id = entry_it->id;
            entry_it = list.erase(entry_it);
            ++count;

            if (receiver) {
                auto rit = by_receiver_.find(receiver);
                if (rit != by_receiver_.end()) {
                    rit->second.erase(id);
                    if (rit->second.empty())
                        by_receiver_.erase(rit);
                }
            }
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

// ── Incoming ──────────────────────────────────────────────────────────────────

void
connection_info::add_sender(object * sender)
{
    std::lock_guard lock { mutex_ };
    senders_[sender]++;
}

void
connection_info::remove_sender(object * sender)
{
    std::lock_guard lock { mutex_ };
    auto it = senders_.find(sender);
    if (it == senders_.end())
        return;
    if (--it->second <= 0)
        senders_.erase(it);
}

void
connection_info::notify_receivers_of_destruction()
{
    // Snapshot receiver pointers while holding our lock, then release before
    // touching receiver objects to avoid lock-ordering issues.
    std::vector<object *> receivers;
    {
        std::lock_guard lock { mutex_ };
        for (auto & [sk, list] : by_sender_) {
            for (auto & entry : list) {
                if (entry.receiver)
                    receivers.push_back(static_cast<object *>(entry.receiver));
            }
        }
    }

    for (auto * recv : receivers)
        if (recv && recv->_nx_connection_info()) {
            // TODO: temporary workaround
            if (recv == owner_)
                continue;

            recv->_nx_connection_info()->remove_sender(owner_);
        }
}

void
connection_info::notify_senders_of_destruction()
{
    // Snapshot sender pointers (keys only) to avoid iterating under lock while
    // senders modify state.
    std::vector<object *> senders_copy;
    {
        std::lock_guard lock { mutex_ };
        senders_copy.reserve(senders_.size());
        for (auto & [sender, _] : senders_)
            senders_copy.push_back(sender);
    }

    for (auto * sender : senders_copy)
        if (sender && sender->_nx_connection_info()) {
            // TODO: temporary workaround
            if (sender == owner_)
                continue;

            sender->_nx_connection_info()->remove_connections_to(
                static_cast<void *>(owner_));
        }
}

} // namespace nx::core
