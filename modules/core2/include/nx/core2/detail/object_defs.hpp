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
// Future extensions (not yet generated):
//   - static meta_object<T> accessor for runtime property introspection
//   - virtual class_name() for polymorphic name query
//

#pragma once

/// Place in the public section of any class deriving from nx::core::object.
///
/// Example:
///   class my_widget : public nx::core::object {
///   public:
///       NX_OBJECT(my_widget)
///       ...
///   };
#define NX_OBJECT(T)                                                             \
    using _nx_self_t = T;                                                        \
    NX_NODISCARD static constexpr const char *                                   \
    static_class_name() noexcept { return #T; }                                  \
    using meta_object_type = ::nx::core::detail::meta_object<T>;                 \
    NX_NODISCARD static meta_object_type &                                       \
    static_meta_object () noexcept                                               \
    { static meta_object_type meta(#T); return meta; }                           \