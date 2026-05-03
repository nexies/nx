//
// connection — type-safe connection flags and entry used by the signal/slot system.
//

#pragma once

#include <nx/common/helpers.hpp>
#include <nx/common/types/enum_flags.hpp>

#include <nx/core2/detail/function_id.hpp>

#include <functional>
#include <memory>

namespace nx::core {

class thread;
class object;

// ──────────────────────────────────────────────────────────────────────────────
// Connection type — how a signal reaches its slot
// ──────────────────────────────────────────────────────────────────────────────

enum class connection_type : std::uint8_t {
    auto_t   = 0,  //< direct if same thread, queued otherwise
    direct   = 1,  //< always synchronous
    queued   = 2,  //< always asynchronous (posted to receiver thread)
    blocking = 3,  //< queued but caller waits for slot to return
};

// ──────────────────────────────────────────────────────────────────────────────
// Connection flags — orthogonal options ORed onto the type
// ──────────────────────────────────────────────────────────────────────────────

enum class connection_flag : std::uint8_t {
    none        = 0x00,
    unique      = 0x10,  //< only one connection allowed for this (sender, signal, receiver, slot) tuple
    single_shot = 0x20,  //< auto-disconnect after the first invocation
};

NX_FLAGS(connection_flag, connection_flags)

// ──────────────────────────────────────────────────────────────────────────────
// connection_entry — internal runtime record for one connection
// ──────────────────────────────────────────────────────────────────────────────

namespace detail {

// Type-erased handler: const void* points to std::tuple<std::decay_t<Args>...>
using connection_handler_t = std::function<void(const void *)>;

struct connection_entry {
    connection_id_t      id              { 0 };
    sender_key_t         sender_key      { 0 };
    void *               receiver        { nullptr }; //< null for free-function / lambda slots
    function_id_t        slot_id         { 0 };
    connection_type      type            { connection_type::auto_t };
    connection_flags     flags           {};
    connection_handler_t handler;
    thread *             receiver_thread { nullptr }; //< null means "no thread tracking"
    // Weak reference to the receiver's liveness sentinel.
    // Empty    → no receiver object (e.g. context-free lambda) — always call.
    // Expired  → receiver has been destroyed — skip and remove.
    std::weak_ptr<void>  receiver_alive;

    NX_NODISCARD bool
    is_unique()      const noexcept { return flags.has(connection_flag::unique); }

    NX_NODISCARD bool
    is_single_shot() const noexcept { return flags.has(connection_flag::single_shot); }
};

} // namespace detail
} // namespace nx::core
