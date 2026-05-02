//
// object_defs — NX_OBJECT macro.
//
// NX_OBJECT(ClassName) must appear in the public section of every class that
// participates in the nx::core2 object system (i.e. derives from nx::core::object).
//
// What it provides:
//
//   _nx_self_t      — type alias for the declaring class, used internally by
//                     NX_SIGNAL and NX_PROPERTY to avoid repeating the class name.
//
//   static_class_name()
//               — constexpr string with the unqualified class name, useful for
//                 logging and debugging without RTTI.
//
//   static_meta_object()
//               — returns the per-class meta_object<T> singleton that holds the
//                 property registry for T.
//
//   get_property(name) / set_property(name, value)
//               — virtual dispatch to the concrete class's property registry,
//                 allowing property access through a base-class pointer without
//                 knowing the derived type.  Both return nx::result<> so callers
//                 can handle unknown names or type-mismatch errors gracefully.
//
//                 NOTE: only properties declared on the *exact* concrete class are
//                 visible — properties from base classes live in their own
//                 meta_object<Base> registry and are not searched automatically.
//                 Full inheritance support requires C++26 static reflection
//                 (std::meta::bases_of) and is deferred until then.
//

#pragma once

#include <any>
#include <string_view>

/// Place in the public section of any class deriving from nx::core::object.
///
/// Example:
///   class my_widget : public nx::core::object {
///   public:
///       NX_OBJECT(my_widget)
///       ...
///   };
#define NX_OBJECT(T)                                                                     \
    using _nx_self_t = T;                                                                \
    NX_NODISCARD static constexpr const char *                                           \
    static_class_name() noexcept { return #T; }                                          \
    using meta_object_type = ::nx::core::detail::meta_object<T>;                         \
    NX_NODISCARD static meta_object_type &                                               \
    static_meta_object() noexcept                                                        \
    { static meta_object_type meta(#T); return meta; }                                   \
    NX_NODISCARD ::nx::result<std::any>                                                  \
    get_property(std::string_view name) override                                         \
    { return T::static_meta_object().property_registry().static_get(name, this); }       \
    ::nx::result<void>                                                                   \
    set_property(std::string_view name, const std::any & value) override                 \
    { return T::static_meta_object().property_registry().static_set(name, this, value); }
