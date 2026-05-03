//
// connection_info — per-object storage for all outgoing connections.
//
// Design:
//   Each object owns a connection_info that stores its outgoing connections
//   indexed by sender_key (sender ptr + signal id).
//
//   Receiver lifetime is tracked via weak_ptr liveness sentinels stored in
//   each connection_entry. When a receiver is destroyed it resets its sentinel
//   shared_ptr, causing all corresponding weak_ptrs to expire. emit() checks
//   each entry before calling and lazily removes dead ones. No cross-object
//   locking is required during destruction.
//

#pragma once

#include <nx/common/helpers.hpp>
#include <nx/core2/detail/function_id.hpp>
#include <nx/core2/object/connection.hpp>

#include <mutex>
#include <unordered_map>
#include <vector>

namespace nx::core {

// ──────────────────────────────────────────────────────────────────────────────
// connection_info
// ──────────────────────────────────────────────────────────────────────────────

class connection_info {
public:
    connection_info() = default;
    ~connection_info() = default;

    NX_DISABLE_COPY(connection_info)

    // ── outgoing (sender side) ────────────────────────────────────────────────

    // Returns false if a unique connection already exists.
    bool
    add_connection(detail::connection_entry entry);

    // Returns false if connection was not found.
    bool
    remove_connection(detail::connection_id_t id);

    // Removes the first connection matching sender_key + receiver ptr + slot_id.
    bool
    remove_connection_by_key(detail::sender_key_t sender_key,
                             void * receiver,
                             detail::function_id_t slot_id);

    // Removes all connections matching sender_key + receiver (any slot).
    // Returns the number of entries removed.
    int
    remove_connections_by_key_and_receiver(detail::sender_key_t sender_key,
                                           void * receiver);

    NX_NODISCARD bool
    has_connection(detail::connection_id_t id) const;

    // Returns a snapshot copy (safe to iterate while connections may change).
    NX_NODISCARD std::vector<detail::connection_entry>
    connections_for(detail::sender_key_t sender_key) const;

private:
    mutable std::mutex mutex_;

    // sender_key → list of entries
    std::unordered_map<detail::sender_key_t,
                       std::vector<detail::connection_entry>> by_sender_;
};

} // namespace nx::core
