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

#include <nx/common.hpp>
#include <nx/common/helpers.hpp>
#include <nx/common/types/result.hpp>

#include <nx/core2/detail/function_id.hpp>
#include <nx/core2/detail/signal_defs.hpp>
#include <nx/core2/detail/property_defs.hpp>
#include <nx/core2/detail/object_defs.hpp>
#include <nx/core2/object/connection.hpp>
#include <nx/core2/object/connection_info.hpp>

#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

namespace nx::core {

class thread;

template<typename Sender, typename SigFn,
    typename Receiver, typename SlotFn>
    bool
    connect(Sender *, SigFn, Receiver *, SlotFn,
        connection_type = connection_type::auto_t, connection_flags = {});

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

    // ── Built-in properties / signals ─────────────────────────────────────────

    NX_OBJECT(object)

    NX_PROPERTY(TYPE std::string, NAME object_name,
                READ object_name, WRITE set_object_name,
                NOTIFY object_name_changed)

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

private:
    class impl;
    std::unique_ptr<impl> impl_;
};



} // namespace nx::core


#include <nx/core2/object/signal_functions.hpp>