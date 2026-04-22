//
// Created by nexie on 4/21/2026.
//

#ifndef NX_PROPERTY_DEFS2_HPP
#define NX_PROPERTY_DEFS2_HPP

/// NX_PROPERTY macro v2
/// =====================
///
/// Purpose:
///   Register a "property" of an enclosing class in the meta-property system
///   (getter, setter, reset, notify signal). Optionally generate storage fields
///   and/or accessor methods directly on the class.
///
/// Syntax:
///   NX_PROPERTY(name, <keywords...>)
///
///
/// ---------------------------------------------------------------------------
/// STORAGE KEYWORDS  (mutually exclusive — at most one)
/// ---------------------------------------------------------------------------
///
///   TYPE <type>
///     Declares the property type and instructs the macro to generate a new
///     data member:
///         <type> m_property_<name>;
///     If DEFAULT <value> is also present, the field gets that initializer.
///     If CONST is also present, the field is declared const (only makes sense
///     with DEFAULT or a constructor-time initializer).
///
///   STORED <member>
///     Points to an already-existing data member that holds the value.
///     The property type is inferred via data_member_trait<decltype(&T::<member>)>
///     (strips ClassType::* to yield the plain MemberType).
///     No new field is generated.
///
///   (neither TYPE nor STORED)
///     No storage is created or assumed. The property is "computed" — its value
///     is produced entirely by the accessor methods supplied via READ / WRITE.
///     In this case READ (and/or WRITE) MUST provide an explicit method name.
///
///
/// ---------------------------------------------------------------------------
/// ACCESSOR KEYWORDS  (independent, each optional)
/// ---------------------------------------------------------------------------
///
///   READ [method]
///
///     READ  (bare, no argument):
///       Generate  auto get_<name>() const;  that reads from the storage field.
///       Requires TYPE or STORED to be present.
///       Registers &T::get_<name> in the meta-property system as getter.
///
///     READ <method>  (explicit existing method name):
///       Do NOT generate a new method. Register &T::<method> as getter instead.
///       When neither TYPE nor STORED is present, the property type is inferred
///       from <method>'s return type.
///
///     (READ absent) + TYPE or STORED present:
///       No method is generated on the class, but a primitive lambda getter
///           [](const T* s) -> auto { return s->m_property_<name>; }
///       is registered in the meta-system for direct field access.
///
///     (READ absent) + no TYPE, no STORED:
///       No getter is registered at all.
///
///
///   WRITE [method]
///
///     WRITE  (bare, no argument):
///       Generate  void set_<name>(const <type>& value);
///       Body: assigns value to the storage field; emits NOTIFY signal if one
///       is registered.  If NOTIFY is absent, just assigns — method still exists.
///       Requires TYPE or STORED to be present.
///       Registers &T::set_<name> in the meta-property system as setter.
///       Incompatible with CONST — compile error if both are present.
///
///     WRITE <method>  (explicit existing method name):
///       Do NOT generate a new method. Register &T::<method> as setter instead.
///       Incompatible with CONST.
///
///     (WRITE absent) + TYPE or STORED present:
///       A primitive lambda setter is registered in the meta-system.
///       No method generated on the class.
///       Still incompatible with CONST — CONST suppresses even primitive setter.
///
///     (WRITE absent) + no TYPE, no STORED:
///       No setter registered.
///
///
///   RESET [method]
///
///     RESET  (bare, no argument):
///       Generate  void reset_<name>();
///       Body: assigns the DEFAULT value (if given) to the storage field, or
///       value-initialises it (<type>{}).  Emits NOTIFY if registered.
///       If NOTIFY is absent, just resets — method still exists.
///       Requires TYPE or STORED to be present.
///       Registers &T::reset_<name> in the meta-property system.
///       Incompatible with CONST — compile error if both are present.
///
///     RESET <method>  (explicit existing method name):
///       Do NOT generate a new method. Register &T::<method> as reset instead.
///       Incompatible with CONST.
///
///     (RESET absent):
///       No reset registered in the meta-system.
///
///
///   NOTIFY [signal]
///
///     NOTIFY  (bare):
///       Generate  NX_SIGNAL(<name>_changed);  on the class and register it.
///       The signal carries the new value as its argument.
///
///     NOTIFY <signal>:
///       Use existing signal <signal> and register it.
///
///     (NOTIFY absent):
///       No notify signal is wired up.  Generated set_<name> / reset_<name>
///       still exist and work — they just do not emit anything.
///
///
///   DEFAULT <value>
///     Only valid together with TYPE.
///     Sets the field's initialiser:
///         <type> m_property_<name>{ <value> };
///     Also used as the reset value inside a generated reset_<name>().
///     If RESET is present but DEFAULT is absent, reset_<name>() value-initialises
///     the field (<type>{}).
///
///
/// ---------------------------------------------------------------------------
/// QUALIFIER KEYWORDS  (flags, no arguments)
/// ---------------------------------------------------------------------------
///
///   CONST
///     Marks the property as immutable — its value is set once (at construction)
///     and cannot be changed afterwards through the meta-property system.
///     Effect:
///       - WRITE / RESET are disallowed (compile error if combined with CONST).
///       - No setter or reset is registered in the meta-system.
///       - If TYPE is present, the generated field is declared const.
///         (Initialise it via a constructor member-initialiser list or DEFAULT.)
///       - The getter is still generated/registered normally.
///     Use for identity-like properties: object ID, creation timestamp, etc.
///
///
///   FINAL                                           [UNDER CONSTRUCTION]
///     Marks the property as non-overridable in derived classes.
///     If a derived class declares NX_PROPERTY with the same name, it is a
///     compile error (or at least a meta-system assertion failure).
///     Mechanism TBD — candidates:
///       a) Inject a deleted / final-tagged sentinel type into the class so
///          that a re-declaration in a subclass causes a redefinition error.
///       b) Record the "final" flag in the meta-descriptor and assert at
///          registration time that no base class already owns this name as final.
///     Either way the flag must be recorded in the meta-descriptor so that
///     tooling and runtime introspection can honour it.
///
///
///   MUTABLE                                         [UNDER CONSTRUCTION]
///     Intended to allow mutation of the property even through a const reference
///     to the owning object (analogous to the C++ `mutable` storage-class).
///     Possible use-case: a lazily-computed cached value that is conceptually
///     const from the outside but needs to be refreshed internally.
///     Interaction with CONST: likely disallowed (contradictory).
///     Exact semantics and code-generation rules TBD.
///
///
/// ---------------------------------------------------------------------------
/// SUMMARY TABLE  (getter column; WRITE/RESET follow the same pattern)
/// ---------------------------------------------------------------------------
///
///  Storage   | READ form    | Field gen? | Method gen?  | Meta getter
///  ----------|--------------|------------|--------------|---------------------
///  TYPE T    | (absent)     | yes        | no           | primitive lambda
///  TYPE T    | READ         | yes        | get_<name>() | &T::get_<name>
///  TYPE T    | READ foo     | yes        | no           | &T::foo
///  STORED m  | (absent)     | no         | no           | primitive lambda
///  STORED m  | READ         | no         | get_<name>() | &T::get_<name>
///  STORED m  | READ foo     | no         | no           | &T::foo
///  (none)    | READ foo     | no         | no           | &T::foo
///  (none)    | READ (bare)  | —          | INVALID      | needs storage
///
///
/// ---------------------------------------------------------------------------
/// EXAMPLES
/// ---------------------------------------------------------------------------
///
///
/// Example 1 — Full auto-generation (most common case)
///
///   NX_PROPERTY(id, TYPE int, READ, WRITE, NOTIFY, RESET, DEFAULT 0)
///
///   // Generates:
///   //   int m_property_id{ 0 };
///   //   int  get_id()  const              { return m_property_id; }
///   //   void set_id(const int& value)     { m_property_id = value; emit id_changed(value); }
///   //   void reset_id()                   { m_property_id = 0;     emit id_changed(0); }
///   //   NX_SIGNAL(id_changed, int)
///   //
///   // Meta: getter=&T::get_id, setter=&T::set_id, reset=&T::reset_id, notify=&T::id_changed
///
///
/// Example 2 — Custom storage, auto accessor methods, existing notify signal
///
///   NX_PROPERTY(name, STORED m_name_, READ, WRITE, NOTIFY name_changed)
///
///   // Generates:
///   //   auto get_name() const                         { return m_name_; }
///   //   void set_name(const decltype(m_name_)& value) { m_name_ = value; emit name_changed(value); }
///   //
///   // Uses existing: m_name_, name_changed
///   // Meta: getter=&T::get_name, setter=&T::set_name, notify=&T::name_changed
///
///
/// Example 3 — Computed property, no storage
///
///   NX_PROPERTY(full_name, READ get_full_name)
///
///   // Uses existing: get_full_name()
///   // Type inferred from get_full_name() return type.
///   // Meta: getter=&T::get_full_name.  No setter, no field.
///
///
/// Example 4 — Minimal: expose existing field to meta-system only
///
///   NX_PROPERTY(score, STORED m_score)
///
///   // Uses existing: m_score  (type via data_member_trait)
///   // Meta: primitive lambda getter + setter for m_score.
///   // No methods added to the class.
///
///
/// Example 5 — Mixed: generated getter, complex existing setter
///
///   NX_PROPERTY(health, STORED m_health, READ, WRITE apply_damage_model, NOTIFY health_changed)
///
///   // Generates: auto get_health() const { return m_health; }
///   // Uses existing: apply_damage_model(), health_changed
///   // Meta: getter=&T::get_health, setter=&T::apply_damage_model, notify=&T::health_changed
///
///
/// Example 6 — Read-only field (no write in meta-system)
///
///   NX_PROPERTY(token, TYPE std::string, READ, NOTIFY token_changed)
///
///   // Generates:
///   //   std::string m_property_token;
///   //   std::string get_token() const { return m_property_token; }
///   //   NX_SIGNAL(token_changed, std::string)
///   //
///   // Meta: getter=&T::get_token, notify=&T::token_changed.  No setter registered.
///
///
/// Example 7 — CONST: immutable property, set at construction only
///
///   NX_PROPERTY(uid, TYPE std::string, READ, CONST)
///
///   // Generates:
///   //   const std::string m_property_uid;   // must be set in ctor init-list
///   //   std::string get_uid() const { return m_property_uid; }
///   //
///   // Meta: getter=&T::get_uid.  No setter or reset — CONST forbids them.
///
///
/// Example 8 — Setter without NOTIFY (method still exists, just doesn't emit)
///
///   NX_PROPERTY(debug_level, TYPE int, READ, WRITE, DEFAULT 0)
///
///   // Generates:
///   //   int m_property_debug_level{ 0 };
///   //   int  get_debug_level() const          { return m_property_debug_level; }
///   //   void set_debug_level(const int& value) { m_property_debug_level = value; }
///   //                                          // no emit — NOTIFY absent
///   // Meta: getter=&T::get_debug_level, setter=&T::set_debug_level
///
///
/// Example 9 — FINAL: prevent derived classes from overriding (under construction)
///
///   NX_PROPERTY(object_type, TYPE std::string, READ, CONST, FINAL)
///
///   // Same generation as CONST example above, plus:
///   // Meta-descriptor records this property as final.
///   // A subclass declaring NX_PROPERTY(object_type, ...) triggers a compile
///   // error (or runtime assertion during meta-registration).
///
/// ---------------------------------------------------------------------------

# pragma region NX_PROPERTY_v2

# include <nx/macro/args/parse2.hpp>

// Arguments ------------------------------------------------------------------

# define _NXPv2_TYPE        0
# define _NXPv2_STORED      1
# define _NXPv2_MEMBER      1
# define _NXPv2_READ        2
# define _NXPv2_WRITE       3
# define _NXPv2_RESET       4
# define _NXPv2_NOTIFY      5
# define _NXPv2_DEFAULT     6
# define _NXPv2_CONST       7
# define _NXPv2_FINAL       8
# define _NXPv2_MUTABLE     9

# define _NXPv2_ARG_MAX     10

# define _NXPv2_DEFAULT_ARGUMENTS \
    _nx_tuple( \
        int,   \
        , \
        , \
        , \


# define _NXPv2(arg) _nx_concat_2(_NXPv2_, arg)

# define _NXPv2_T(arg) _nx_args_tokenize(_NXPv2(arg))

# define _NXPv2_T_ALL_ITERATOR(n, a) \
    _NXPv2_T(a)

# define _NXPv2_T_ALL(...) \
    _nx_make_decorated_iterate(_NXPv2_T_ALL_ITERATOR,_nx_sequence_comma_separator, __VA_ARGS__)

# define _NXPv2_SORT_COND_D(d, res, ...) \
    NX_HAS_ARGS(__VA_ARGS__)

# define _NXPv2_SORT_OP_D(d, res, cur, ...) \
    _nx_logic_if(_nx_args_token_has_value(cur)) ( \
        _nx_expand( \
            _nx_tuple_set(res, _nx_args_token_name(cur), _nx_args_token_value(cur))   \
            _nx_append_args(__VA_ARGS__)) , \
        _nx_expand( \
            _nx_tuple_set(res, _nx_args_token_name(cur), 1) \
            _nx_append_args(__VA_ARGS__)) \
        )

# define _NXPv2_SORT_RES_D(d, res, ...) \
    _nx_tuple_unpack(res)

# define _NXPv2_SORT_D(d, /*tokens*/ ...)

# pragma endregion

#endif //NX_PROPERTY_DEFS2_HPP
