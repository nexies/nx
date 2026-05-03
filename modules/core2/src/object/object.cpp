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
        : owner_     (owner)
        , conn_info_ (std::make_shared<connection_info>())
        , liveness_  (std::make_shared<bool>(true))
    {}

    ~impl()
    {
        // Unlink from parent first so the parent won't attempt to delete us
        // when its own destructor runs (handles stack-allocated children that
        // are destroyed before their parent).
        if (parent_) {
            parent_->on_child_removed(owner_);
            parent_->impl_->remove_child(owner_);
        }

        // conn_info_ shared_ptr is released here. Any emit() that grabbed its
        // own shared_ptr copy will keep the table alive until it finishes.

        // Destroy owned (heap) children in reverse construction order.
        // Null out child's parent_ pointer before deleting so the child's own
        // ~impl() does not try to call back into us while we are mid-destruction.
        while (!children_.empty()) {
            auto * child = children_.back();
            children_.pop_back();
            child->impl_->parent_ = nullptr;
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

    // Connection table, shared so emit() can keep it alive across destruction.
    std::shared_ptr<connection_info> conn_info_;

    // Liveness sentinel: reset at the very start of ~object() so all
    // weak_ptr<void> stored in connection entries expire immediately. Any
    // in-flight emit() on any sender will then skip calls to this receiver.
    std::shared_ptr<void> liveness_;
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
    // Expire all receiver_alive weak_ptrs BEFORE emitting destroyed.
    // This ensures that no slot on this object can be invoked by any
    // in-flight or future emit() call after this point.
    impl_->liveness_.reset();

    NX_EMIT(destroyed);
    // impl_ destructor handles children and releases conn_info_ shared_ptr.
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
    if (impl_->parent_) {
        impl_->parent_->on_child_removed(this);
        impl_->parent_->impl_->remove_child(this);
    }

    impl_->parent_ = new_parent;

    if (new_parent) {
        new_parent->impl_->add_child(this);
        new_parent->on_child_added(this);

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
    return impl_->conn_info_.get();
}

const connection_info *
object::_nx_connection_info() const noexcept
{
    return impl_->conn_info_.get();
}

std::shared_ptr<connection_info>
object::_nx_connection_info_shared() noexcept
{
    return impl_->conn_info_;
}

std::weak_ptr<void>
object::_nx_liveness_token() const noexcept
{
    return impl_->liveness_;
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

void object::on_child_added  (object *) {}
void object::on_child_removed(object *) {}

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

// ── property access ───────────────────────────────────────────────────────────

nx::result<std::any>
object::get_property(std::string_view name)
{
    return object::static_meta_object().property_registry().static_get(name, this);
}

nx::result<void>
object::set_property(std::string_view name, const std::any & value)
{
    return object::static_meta_object().property_registry().static_set(name, this, value);
}

// ── event dispatch ────────────────────────────────────────────────────────────

bool
object::on_event(event &)
{
    return false;
}

bool
object::send_event(object * target, event & e)
{
    if (!target) return false;
    return target->on_event(e);
}

} // namespace nx::core
