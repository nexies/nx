//
// connection_info — per-object storage for all outgoing and incoming connections.
//

#pragma once

#include <nx/common/helpers.hpp>
#include <nx/core2/detail/function_id.hpp>
#include <nx/core2/object/connection.hpp>

#include <mutex>
#include <set>
#include <unordered_map>
#include <vector>

namespace nx::core {

class object;

// ──────────────────────────────────────────────────────────────────────────────
// connection_info
//
// Owned by each object through object::impl.
// Thread-safe: all public methods lock mutex_.
//
// Layout:
//   by_sender_  — connections indexed by sender_key (sender ptr + signal id).
//                 Used by emit() to quickly find all slots for a signal.
//   by_receiver_ — connection IDs indexed by receiver ptr.
//                  Used on receiver destruction to clean up connections.
//   senders_     — objects whose signals this object is listening to.
//                  Used on this object's destruction to notify senders.
// ──────────────────────────────────────────────────────────────────────────────

class connection_info {
public:
    explicit connection_info(object * owner);
    ~connection_info();

    NX_DISABLE_COPY(connection_info)

    // ── outgoing (sender side) ────────────────────────────────────────────────

    // Returns false if a unique connection already exists.
    bool
    add_connection(detail::connection_entry entry);

    // Returns false if connection was not found.
    bool
    remove_connection(detail::connection_id_t id);

    // Removes the first connection matching sender_key + receiver ptr + slot_id.
    // Used by disconnect() since IDs are now unique counters, not content hashes.
    bool
    remove_connection_by_key(detail::sender_key_t sender_key,
                             void * receiver,
                             detail::function_id_t slot_id);

    // Remove all connections to a specific receiver pointer.
    void
    remove_connections_to(void * receiver);

    // Removes all connections matching sender_key + receiver (any slot).
    // Returns the number of entries removed (used for sender-tracking refcount).
    int
    remove_connections_by_key_and_receiver(detail::sender_key_t sender_key,
                                           void * receiver);

    NX_NODISCARD bool
    has_connection(detail::connection_id_t id) const;

    // Returns a snapshot copy (safe to iterate while connections may change).
    NX_NODISCARD std::vector<detail::connection_entry>
    connections_for(detail::sender_key_t sender_key) const;

    // ── incoming (receiver side) ──────────────────────────────────────────────

    void
    add_sender(object * sender);

    void
    remove_sender(object * sender);

    // Called when this object (as receiver) is being destroyed.
    // Walks the senders_ set and asks each sender to drop connections to us.
    void
    notify_senders_of_destruction();

    // Called when this object (as sender) is being destroyed.
    // Walks outgoing connections and removes this object from each receiver's
    // senders_ set, so receivers won't try to access us after we're gone.
    void
    notify_receivers_of_destruction();

private:
    object *  owner_;
    mutable std::mutex mutex_;

    // sender_key → list of entries
    std::unordered_map<detail::sender_key_t,
                       std::vector<detail::connection_entry>> by_sender_;

    // receiver ptr → set of connection IDs (for fast lookup on receiver death)
    std::unordered_map<void *, std::set<detail::connection_id_t>> by_receiver_;

    // objects whose signals we're listening to, with a reference count.
    // An object appears once per live connection (not once per sender object),
    // so disconnecting one connection does not erase tracking for others.
    std::unordered_map<object *, int> senders_;
};

} // namespace nx::core
