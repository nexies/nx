//
// object — implementation of nx::core::object
//

#include <nx/core2/object/object.hpp>
#include <nx/core2/object/connection_info.hpp>
#include <nx/core2/thread/thread.hpp>

#include <algorithm>
#include <cassert>
#include <vector>

namespace nx::core {

// ──────────────────────────────────────────────────────────────────────────────
// object::impl
// ──────────────────────────────────────────────────────────────────────────────

class object::impl {
public:
    explicit impl(object * owner)
        : owner_   (owner)
        , conn_info_(owner)
    {}

    ~impl()
    {
        // Disconnect from all senders before destroying children so that
        // no signal fires on a half-destroyed tree.
        conn_info_.notify_senders_of_destruction();

        // Destroy children in reverse construction order.
        while (!children_.empty()) {
            auto * child = children_.back();
            children_.pop_back();
            delete child;
        }
    }

    void
    add_child(object * child)
    {
        assert(child && "add_child: null child");
        children_.push_back(child);
    }

    void
    remove_child(object * child)
    {
        auto it = std::find(children_.begin(), children_.end(), child);
        if (it != children_.end())
            children_.erase(it);
    }

    object *              owner_;
    object *              parent_  { nullptr };
    std::vector<object *> children_;
    thread *              thread_  { nullptr };
    object *              sender_  { nullptr };
    connection_info       conn_info_;
};

// ──────────────────────────────────────────────────────────────────────────────
// object
// ──────────────────────────────────────────────────────────────────────────────

object::object(object * parent)
    : impl_(std::make_unique<impl>(this))
{
    // Associate with the current thread by default
    impl_->thread_ = thread::current();

    if (parent)
        set_parent(parent);
}

object::~object()
{
    NX_EMIT(destroyed);
    // impl_ destructor handles children and connection cleanup
}

// ── Thread affinity ───────────────────────────────────────────────────────────

thread *
object::get_thread() const noexcept
{
    return impl_->thread_;
}

std::uint32_t
object::thread_id() const noexcept
{
    auto * t = impl_->thread_;
    return t ? t->id() : invalid_thread_id;
}

nx::result<void>
object::move_to_thread(thread * t)
{
    if (!t)
        return nx::error { std::make_error_code(std::errc::invalid_argument), "move_to_thread: null thread" };

    thread * old = impl_->thread_;
    if (old == t)
        return {};

    impl_->thread_ = t;
    _on_thread_changed(old, t);

    // Recursively move children
    for (auto * child : impl_->children_)
        child->move_to_thread(t);

    return {};
}

// ── Object tree ───────────────────────────────────────────────────────────────

object *
object::parent() const noexcept
{
    return impl_->parent_;
}

void
object::set_parent(object * new_parent)
{
    if (impl_->parent_ == new_parent)
        return;

    // Remove from current parent
    if (impl_->parent_)
        impl_->parent_->impl_->remove_child(this);

    impl_->parent_ = new_parent;

    if (new_parent) {
        new_parent->impl_->add_child(this);

        // Inherit thread affinity from parent if not already set
        if (!impl_->thread_ && new_parent->impl_->thread_)
            move_to_thread(new_parent->impl_->thread_);
    }
}

std::vector<object *>
object::children() const
{
    return impl_->children_;
}

// ── Sender introspection ──────────────────────────────────────────────────────

object *
object::sender() const noexcept
{
    return impl_->sender_;
}

// ── Connection info ───────────────────────────────────────────────────────────

connection_info *
object::_nx_connection_info() noexcept
{
    return &impl_->conn_info_;
}

const connection_info *
object::_nx_connection_info() const noexcept
{
    return &impl_->conn_info_;
}

// ── Protected helpers ─────────────────────────────────────────────────────────

void
object::_set_sender(object * s) noexcept
{
    impl_->sender_ = s;
}

void
object::_on_thread_changed(thread * /*old_thread*/, thread * /*new_thread*/)
{
    // Base implementation: no-op.
    // Subclasses (e.g. timer) may need to re-arm timers on the new context.
}

// ──────────────────────────────────────────────────────────────────────────────
// detail::is_same_thread / detail::post_to_thread
// Defined here because this TU includes both object.hpp and thread.hpp.
// signal_functions.hpp uses only a forward declaration of thread.
// ──────────────────────────────────────────────────────────────────────────────

namespace detail {

bool
is_same_thread(thread * t) noexcept
{
    return !t || (t == thread::current());
}

void
post_to_thread(thread * t, std::function<void()> task)
{
    t->post(std::move(task));
}

} // namespace detail

} // namespace nx::core
