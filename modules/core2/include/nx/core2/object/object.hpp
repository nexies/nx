//
// object — base class for all objects in the nx::core object system.
//
// Features:
//   - Object tree: parent/child ownership (parent destroys children)
//   - Thread affinity: every object belongs to one thread
//   - Signal/slot: connect(), disconnect(), emit() free functions
//   - Properties: NX_PROPERTY macro
//   - Signals: NX_SIGNAL macro
//

#pragma once

#include <nx/common/helpers.hpp>
#include <nx/common/types/result.hpp>

#include <nx/core2/detail/signal_defs.hpp>
#include <nx/core2/detail/property_defs2.hpp>
#include <nx/core2/detail/object_defs.hpp>
#include <nx/core2/object/connection.hpp>
#include <nx/core2/object/connection_info.hpp>
#include <nx/core2/object/meta_object.hpp>
#include <nx/core2/event/event.hpp>


#include <string>
#include <type_traits>
#include <vector>

namespace nx::core {

class thread;

    template<typename Sender, typename SigFn,
    typename Receiver, typename SlotFn>
    bool
    connect(Sender *, SigFn, Receiver *, SlotFn,
        connection_type = connection_type::auto_t, connection_flags = connection_flag::none);

    template<typename Sender, typename SigFn,
             typename Receiver, typename SlotFn>
    bool
    disconnect(Sender *, SigFn, Receiver *, SlotFn);

    template<typename Sender, typename SigFn, typename... Args>
    void
    emit(Sender *, SigFn, Args &&...);


// ──────────────────────────────────────────────────────────────────────────────
// object
// ──────────────────────────────────────────────────────────────────────────────

class object {
    friend class connection_info;

    template<typename Sender, typename SigFn,
             typename Receiver, typename SlotFn>
    friend bool
    connect(Sender *, SigFn, Receiver *, SlotFn,
            connection_type, connection_flags);

    template<typename Sender, typename SigFn,
             typename Receiver, typename SlotFn>
    friend bool
    disconnect(Sender *, SigFn, Receiver *, SlotFn);

    template<typename Sender, typename SigFn, typename... Args>
    friend void
    emit(Sender *, SigFn, Args &&...);

public:
    explicit object(object * parent = nullptr);
    virtual ~object();

    NX_DISABLE_COPY(object)

    // ── Thread affinity ───────────────────────────────────────────────────────

    NX_NODISCARD thread *
    get_thread() const noexcept;

    NX_NODISCARD std::uint32_t
    thread_id() const noexcept;

    // Move this object (and all children) to another thread.
    // Must be called from the object's current thread or before the first event loop.
    nx::result<void>
    move_to_thread(thread * t);

    // ── Object tree ───────────────────────────────────────────────────────────

    NX_NODISCARD object *
    parent() const noexcept;

    // Re-parents this object. If new_parent is null, object becomes root.
    // Ownership semantics: a non-null parent will delete this object in its destructor.
    void
    set_parent(object * new_parent);

    NX_NODISCARD std::vector<object *>
    children() const;

    // ── Introspection ─────────────────────────────────────────────────────────

    // Returns the object that emitted the signal currently being processed.
    // Valid only during a slot call; null otherwise.
    NX_NODISCARD object *
    sender() const noexcept;

    // ── Event dispatch ────────────────────────────────────────────────────────
    //
    // Lightweight, synchronous, virtual event delivery — no signal overhead.
    // Override on_event() in subclasses to handle specific event types.
    // send_event() is a static helper that calls target->on_event(e).

    virtual bool on_event(event & e);

    static bool send_event(object * target, event & e);

    // ── Built-in properties / signals ─────────────────────────────────────────

    NX_OBJECT(object)

    NX_PROPERTY(object_name, TYPE std::string, READ, WRITE, NOTIFY)

    NX_SIGNAL(destroyed)

    // ── Internal (used by friend templates) ───────────────────────────────────

    NX_NODISCARD connection_info *
    _nx_connection_info() noexcept;

    NX_NODISCARD const connection_info *
    _nx_connection_info() const noexcept;

protected:
    // Allows subclasses to obtain the current signal sender.
    void
    _set_sender(object * s) noexcept;

    // Called during move_to_thread, recursively moves children.
    virtual void
    _on_thread_changed(thread * old_thread, thread * new_thread);

    // Called on the parent when a child is added or removed via set_parent().
    // Also called on the old parent when the child's destructor runs.
    // Default implementations are no-ops.
    virtual void on_child_added  (object * child);
    virtual void on_child_removed(object * child);

private:
    class impl;
    std::unique_ptr<impl> impl_;
};



} // namespace nx::core


#include <nx/core2/object/signal_functions.inl>
