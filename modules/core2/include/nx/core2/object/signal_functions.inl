//
// signal_functions — emit(), connect(), disconnect() free function templates.
//
// thread.hpp is NOT included here to avoid circular dependency:
//   object.hpp → signal_functions.hpp → thread.hpp → object.hpp
//
// Thread-dispatch is delegated to two opaque helpers defined in object.cpp,
// which is the only TU that must include both object.hpp and thread.hpp.
//

#pragma once

#include <nx/core2/object/object.hpp>

#include <functional>
#include <memory>
#include <tuple>
#include <type_traits>

namespace nx::core {

class thread; // forward declaration — full type not needed here

// ──────────────────────────────────────────────────────────────────────────────
// Internal dispatch helpers — implemented in object.cpp (includes thread.hpp)
// ──────────────────────────────────────────────────────────────────────────────

namespace detail {

// Returns true when t is null (no affinity) or t == thread::current().
bool is_same_thread(thread * t) noexcept;

// Posts task to t's event-loop context.
void post_to_thread(thread * t, std::function<void()> task);

} // namespace detail

// ──────────────────────────────────────────────────────────────────────────────
// connect
// ──────────────────────────────────────────────────────────────────────────────

template<typename Sender, typename SigFn,
         typename Receiver, typename SlotFn,
         typename>
bool
connect(Sender *         sender,
        SigFn            signal,
        Receiver *       receiver,
        SlotFn           slot,
        connection_type  type,
        connection_flags flags)
{
    static_assert(std::is_base_of_v<object, Sender>,
                  "connect(): Sender must derive from nx::core::object");

    using sig_traits  = detail::function_traits<std::decay_t<SigFn>>;
    using slot_traits = detail::function_traits<std::decay_t<SlotFn>>;
    static_assert(std::is_same_v<typename sig_traits::args_tuple,
                                  typename slot_traits::args_tuple>,
                  "connect(): signal and slot must have matching argument types");

    using args_tuple_t = typename sig_traits::args_tuple;

    const auto sig_id  = detail::get_function_id(signal);
    const auto slot_id = detail::get_function_id(slot);
    const auto conn_id = detail::make_connection_id();
    const auto sk      = detail::make_sender_key(sender, sig_id);

    detail::connection_handler_t handler =
        [recv = receiver, slot_fn = slot](const void * args_ptr)
        {
            const auto & args = *static_cast<const args_tuple_t *>(args_ptr);
            std::apply([recv, slot_fn](auto &&... a) {
                (recv->*slot_fn)(std::forward<decltype(a)>(a)...);
            }, args);
        };

    // get_thread() returns thread* — forward decl is enough
    thread * recv_thread = nullptr;
    if constexpr (std::is_base_of_v<object, Receiver>)
        recv_thread = static_cast<object *>(receiver)->get_thread();

    detail::connection_entry entry;
    entry.id              = conn_id;
    entry.sender_key      = sk;
    entry.receiver        = static_cast<void *>(receiver);
    entry.slot_id         = slot_id;
    entry.type            = type;
    entry.flags           = flags;
    entry.handler         = std::move(handler);
    entry.receiver_thread = recv_thread;

    auto * sender_info = static_cast<object *>(sender)->_nx_connection_info();
    if (!sender_info->add_connection(std::move(entry)))
        return false;

    if constexpr (std::is_base_of_v<object, Receiver>)
        static_cast<object *>(receiver)->_nx_connection_info()
            ->add_sender(static_cast<object *>(sender));

    return true;
}

// ── connect (lambda / callable overload) ─────────────────────────────────────
//
// Connects a signal to a callable (lambda, functor, etc.).
// `context` is an object whose lifetime governs the connection — when it is
// destroyed, the connection is automatically removed.  Pass nullptr to create
// a context-free (potentially dangling) connection.
//
// Use connection_flag::single_shot to auto-disconnect after the first call.
// Use disconnect(sender, signal, context, nullptr) to remove all callable
// connections from this (sender, signal, context) triple.

template<typename Sender, typename SigFn, typename Callable,
         typename>
bool
connect(Sender *         sender,
        SigFn            signal,
        object *         context,
        Callable &&      slot,
        connection_type  type,
        connection_flags flags)
{
    static_assert(std::is_base_of_v<object, Sender>,
                  "connect(): Sender must derive from nx::core::object");

    using sig_traits   = detail::function_traits<std::decay_t<SigFn>>;
    using args_tuple_t = typename sig_traits::args_tuple;

    const auto sig_id  = detail::get_function_id(signal);
    const auto slot_id = detail::get_function_id(slot); // before forward/move
    const auto conn_id = detail::make_connection_id();
    const auto sk      = detail::make_sender_key(sender, sig_id);

    // Capture callable by value so it outlives this call site.
    detail::connection_handler_t handler =
        [fn = std::forward<Callable>(slot)](const void * args_ptr) mutable
        {
            const auto & args = *static_cast<const args_tuple_t *>(args_ptr);
            std::apply([&fn](auto &&... a) {
                fn(std::forward<decltype(a)>(a)...);
            }, args);
        };

    thread * ctx_thread = context ? context->get_thread() : nullptr;

    detail::connection_entry entry;
    entry.id              = conn_id;
    entry.sender_key      = sk;
    entry.receiver        = static_cast<void *>(context);
    entry.slot_id         = slot_id;
    entry.type            = type;
    entry.flags           = flags;
    entry.handler         = std::move(handler);
    entry.receiver_thread = ctx_thread;

    auto * sender_info = static_cast<object *>(sender)->_nx_connection_info();
    if (!sender_info->add_connection(std::move(entry)))
        return false;

    if (context)
        context->_nx_connection_info()->add_sender(static_cast<object *>(sender));

    return true;
}

// Convenience overloads

template<typename Sender, typename SigFn, typename Receiver, typename SlotFn>
bool connect_direct(Sender * sender, SigFn signal, Receiver * receiver, SlotFn slot)
{
    return connect(sender, signal, receiver, slot, connection_type::direct);
}

template<typename Sender, typename SigFn, typename Receiver, typename SlotFn>
bool connect_queued(Sender * sender, SigFn signal, Receiver * receiver, SlotFn slot)
{
    return connect(sender, signal, receiver, slot, connection_type::queued);
}

template<typename Sender, typename SigFn, typename Receiver, typename SlotFn>
bool connect_unique(Sender * sender, SigFn signal, Receiver * receiver, SlotFn slot)
{
    return connect(sender, signal, receiver, slot,
                   connection_type::auto_t, connection_flag::unique);
}

// ──────────────────────────────────────────────────────────────────────────────
// disconnect
// ──────────────────────────────────────────────────────────────────────────────

template<typename Sender, typename SigFn,
         typename Receiver, typename SlotFn>
bool
disconnect(Sender * sender, SigFn signal, Receiver * receiver, SlotFn slot)
{
    const auto sig_id  = detail::get_function_id(signal);
    const auto slot_id = detail::get_function_id(slot);
    const auto sk      = detail::make_sender_key(sender, sig_id);

    // Connection IDs are now unique counters — find first match by (receiver, slot_id).
    auto * sender_info = static_cast<object *>(sender)->_nx_connection_info();
    if (!sender_info->remove_connection_by_key(
            sk, static_cast<void *>(receiver), slot_id))
        return false;

    if constexpr (std::is_base_of_v<object, Receiver>)
        static_cast<object *>(receiver)->_nx_connection_info()
            ->remove_sender(static_cast<object *>(sender));

    return true;
}

// ── disconnect (remove all callable connections for a context) ────────────────
//
// Removes every callable connection on (sender, signal) → context.
// Pass nullptr as the last argument when you don't have the original callable.

template<typename Sender, typename SigFn>
bool
disconnect(Sender * sender, SigFn signal, object * context, std::nullptr_t)
{
    const auto sig_id = detail::get_function_id(signal);
    const auto sk     = detail::make_sender_key(sender, sig_id);

    auto * sender_info = static_cast<object *>(sender)->_nx_connection_info();
    const int count = sender_info->remove_connections_by_key_and_receiver(
        sk, static_cast<void *>(context));

    if (context) {
        for (int i = 0; i < count; ++i)
            context->_nx_connection_info()->remove_sender(static_cast<object *>(sender));
    }
    return count > 0;
}

// ──────────────────────────────────────────────────────────────────────────────
// emit
// ──────────────────────────────────────────────────────────────────────────────

template<typename Sender, typename SigFn, typename... Args>
void
emit(Sender * sender, SigFn signal, Args &&... args)
{
    static_assert(std::is_base_of_v<object, Sender>,
                  "emit(): Sender must derive from nx::core::object");

    using args_tuple_t = std::tuple<std::decay_t<Args>...>;

    const auto sig_id = detail::get_function_id(signal);
    const auto sk     = detail::make_sender_key(sender, sig_id);

    auto * conn_info = static_cast<object *>(sender)->_nx_connection_info();
    if (!conn_info)
        return;

    const auto connections = conn_info->connections_for(sk);
    if (connections.empty())
        return;

    args_tuple_t args_tuple(std::forward<Args>(args)...);

    for (const auto & entry : connections) {
        const bool call_direct =
            (entry.type == connection_type::direct) ||
            (entry.type == connection_type::auto_t &&
             detail::is_same_thread(entry.receiver_thread));

        if (call_direct) {
            if (entry.receiver) {
                auto * recv_obj = static_cast<object *>(entry.receiver);
                recv_obj->_set_sender(static_cast<object *>(sender));
                entry.handler(static_cast<const void *>(&args_tuple));
                recv_obj->_set_sender(nullptr);
            } else {
                entry.handler(static_cast<const void *>(&args_tuple));
            }
        } else {
            auto boxed = std::make_shared<args_tuple_t>(args_tuple);
            auto sndr  = static_cast<object *>(sender);
            auto recv  = static_cast<object *>(entry.receiver);
            auto task  = [h = entry.handler, b = std::move(boxed),
                          sndr, recv]() mutable
            {
                if (recv) recv->_set_sender(sndr);
                h(static_cast<const void *>(b.get()));
                if (recv) recv->_set_sender(nullptr);
            };
            detail::post_to_thread(entry.receiver_thread, std::move(task));
        }

        if (entry.is_single_shot())
            conn_info->remove_connection(entry.id);
    }
}

} // namespace nx::core
