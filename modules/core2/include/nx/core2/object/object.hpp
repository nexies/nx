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


#include <any>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

namespace nx::core {

class thread;

    template<typename Sender, typename SigFn,
             typename Receiver, typename SlotFn,
             typename = typename std::enable_if_t<std::is_member_function_pointer_v<std::decay_t<SlotFn>>, int>>
    bool
    connect(Sender *, SigFn, Receiver *, SlotFn,
            connection_type = connection_type::auto_t,
            connection_flags = connection_flag::none);

    template<typename Sender, typename SigFn, typename Callable,
             typename = typename std::enable_if_t<!std::is_member_function_pointer_v<std::decay_t<Callable>>, int>>
    bool
    connect(Sender *, SigFn, object *, Callable &&,
            connection_type = connection_type::auto_t,
            connection_flags = connection_flag::none);

    template<typename Sender, typename SigFn>
    bool
    disconnect(Sender *, SigFn, object *, std::nullptr_t);

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
             typename Receiver, typename SlotFn,
             typename>
    friend bool
    connect(Sender *, SigFn, Receiver *, SlotFn,
            connection_type, connection_flags);

    template<typename Sender, typename SigFn, typename Callable,
             typename>
    friend bool
    connect(Sender *, SigFn, object *, Callable &&,
            connection_type, connection_flags);

    template<typename Sender, typename SigFn>
    friend bool
    disconnect(Sender *, SigFn, object *, std::nullptr_t);

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

    // Manual expansion of NX_OBJECT(object) — object is the root of the hierarchy
    // so the virtual declarations must live here; all derived classes use
    // NX_OBJECT(T) which generates the override implementations.
    using _nx_self_t      = object;
    using meta_object_type = ::nx::core::detail::meta_object<object>;
    NX_NODISCARD static constexpr const char * static_class_name() noexcept { return "object"; }
    NX_NODISCARD static meta_object_type & static_meta_object() noexcept
    { static meta_object_type meta("object"); return meta; }

    // Virtual property access. Overridden in every NX_OBJECT(T) subclass so
    // the concrete class's own meta_property_registry is queried.
    //
    // NOTE: each class's registry only contains properties declared in *that*
    // class.  Full inheritance chain search is deferred until C++26 static
    // reflection (std::meta::bases_of) becomes available.
    NX_NODISCARD virtual nx::result<std::any>
    get_property(std::string_view name);

    virtual nx::result<void>
    set_property(std::string_view name, const std::any & value);

    NX_PROPERTY(object_name, TYPE std::string, READ, WRITE, NOTIFY)

    NX_SIGNAL(destroyed)

    // ── Internal (used by friend templates) ───────────────────────────────────

    NX_NODISCARD connection_info *
    _nx_connection_info() noexcept;

    NX_NODISCARD const connection_info *
    _nx_connection_info() const noexcept;

    // Returns a shared_ptr to the connection table. Used by emit() to keep
    // the table alive even if this object is destroyed mid-emission.
    NX_NODISCARD std::shared_ptr<connection_info>
    _nx_connection_info_shared() noexcept;

    // Returns a weak_ptr to the liveness sentinel. Stored in connection entries
    // so emit() can check if the receiver is still alive before calling its slot.
    NX_NODISCARD std::weak_ptr<void>
    _nx_liveness_token() const noexcept;

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
