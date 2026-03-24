//
// Created by nexie on 19.12.2025.
//

#ifndef PROPERTY_DEFS_CXX20_HPP
#define PROPERTY_DEFS_CXX20_HPP

#ifndef __nx_cxx_20_property
#error ""
#else

///
/// Plan for improving the usability of NX_PROPERTY:
///
///     1) NX_PROPERTY should not only declare new member functions for accessing a certain member field, but also should
///         define a meta-struct, defining the property's meta information
///     2) NX_PROPERTY should be able to use existing member functions and fields to define object's property.
///     This is especially useful when setters/getters are non-trivial.
///     For example:
///
///         struct Auth : public Object
///         {
///             private:
///             std::size_t password_hash;
///
///             public:
///
///             void setPassword(std::string new_password)
///             {
///                 password_hash = std::hash<std::string>{}(new_password);
///             }
///
///             NX_PROPERTY(TYPE std::size_t, MEMBER NAME password_hash, MEMBER WRITE setPassword);
///         }
///
///         NX_PROPERTY macro should expand into something like this:
///         struct : public :: nx::property_descriptor_t
///         {
///
///         }


///
/// struct Creature
/// {
///     NX_ENABLE_PROPERTIES(Creature)
///
///     NX_PROPERTY(TYPE float, NAME weight, READ weight, CONST, DEFAULT 40)
///         // 1) Creates member property descriptor - name "weight", type "float"
///         // 2) Creates member data field "const float m_weight" ???
///         // 3) Creates member getter - name "weight", const, return type "float"
///
///
///    Creature(float weight = propertyDefault<"weight">()) : m_weight(weight) {}
/// }
///
/// struct Person : public Creature
/// {
///     NX_ENABLE_PROPERTIES(Person)
///
///     NX_PROPERTY(TYPE int, NAME id, READ id, WRITE setId)
///         // 1) Creates member property descriptor - name "id", type "int"
///         // 2) Creates member data field "int m_id";    ???
///         // 3) Creates member getter - name "id", const, return type std::string
///         // 4) Creates member setter - name "setId", argument const "std::string &", return void
///
///     struct Initials
///     {
///         std::string name;
///         std::string surname;
///     } m_initials;
///
///     std::string getName () const { return m_initials.name; }
///     std::string setName (std::string name) const { m_initials.name = name; }
///
///     NX_PROPERTY(TYPE std::string, NAME firstName, STORED false, MEMBER READ getName, MEMBER WRITE setName)
///         // 1) Creates member property descriptor - name "firstName", type "std::string"
///         // 2) Does not create getter - uses existing getter
///         // 3) Does not create setter - uses existing setter
///
///    Person() : Creature(80) {}
/// }
///
///     Person p;
///     p.setName("Phil");
///
///     p.property<"firstName">();  // calls Person::getName, returns "Phil";
///     p.setProperty<"firstName">("John") // calls Person::setName
///     p.property<"weight">(); // calls Creature::weight, returns 80
///     p.propertyDefault<"firstName">() // Undefined or returns {} ???
///

///     Parameters for NX_PROPERTY macro:
///         Mandatory parameters:
///             - TYPE <type>               : type of the property
///             - NAME <name>               : name of the property
///         Optional parameters:
///         [ property descriptor ]:
///             - DESCRIPTOR_TYPE           : type of the nested property-descriptor structure
///                 * if absent, defaults to property_##<name>##_t
///             - DESCRIPTOR_NAME           : name of the nested property-descriptor structure
///                 * if absent, default to m_property_##<name>
///
///         [ getter ]:
///             - READ <function>           : auto-generated getter function
///                 * conflicts with USING_READ parameter
///                 * conflicts with MEMBER parameter if [setter] is present
///
///             - USING_READ <function>     : member getter function
///                 * conflicts with READ parameter
///                 * conflicts with MEMBER parameter if [setter] is present
///
///         [ setter ]:
///             - WRITE <function>          : auto-generated setter function
///                 * conflicts with USING_WRITE parameter
///                 * conflicts with MEMBER parameter if [getter] is present
///
///             - USING_WRITE <function>    : member setter function
///                 * conflicts with WRITE parameter
///                 * conflicts with MEMBER parameter if [getter] is present
///
///         [ access ]:
///             - MEMBER <field>            : member data field of the class, storing the property
///                 * if absent, value of the property will not be contained inside the class directly
///
///         [ notify ]:
///             - NOTIFY <signal>           : auto-generated member NX_SIGNAL change notifier
///                 * conflicts with USING_NOTIFY parameter
///                 * if [setter] is absent and MEMBER is present is still emit on property value change
///
///             - USING_NOTIFY <signal>     : existing member NX_SIGNAL change notifier
///                 * conflicts with NOTIFY
///                 * if [setter] is absent and MEMBER is present is still emit on property value change
///
///         [ reset ]:
///             - RESET <reset>             : auto-generated reset member function
///                 * conflicts with USING_RESET parameter
///
///             - USING_RESET <reset>       : exiting reset member function
///                 * conflicts with RESET parameter
///                 * conflicts with DEFAULT parameter (?)
///
///         [ additional ]:
///             - DEFAULT <value>           : default value for the property
///                 * defaults to <type>{} if not present
///
///             - CONSTANT                  : const modifier on the property
///                 * if present, conflicts with [setter] and [reset]
///
///             - MUTABLE                   : mutable modifier on the property
///
///             - FINAL                     : final modifier on the property
///                 * indicates that the property shall not be overwritten by a derived class
///

#include <nx/core/detail/property_defs_cxx20_parser.hpp>

#define _nx_cxx_20_property_rule_set \
    NX_ARGS_RULE_SET( \
        \
        NX_ARGS_RULE_NOT_EMPTY_S( \
            "Invalid parameters: needs at least TYPE and NAME parameter") \
        , \
        NX_ARGS_RULE_UNIQUE_VALUE_S(TYPE, \
            "Invalid parameter: TYPE must appear exactly once and name a valid C++ type") \
        , \
        NX_ARGS_RULE_UNIQUE_VALUE_S(NAME, \
            "Invalid parameter: NAME must appear exactly once and be a valid C++ identifier") \
        , \
        NX_ARGS_RULE_UNIQUE_VALUE_S(DESCRIPTOR_TYPE, \
            "Invalid parameter: DESCRIPTOR_TYPE must appear exactly once and name a valid C++ type") \
        , \
        NX_ARGS_RULE_UNIQUE_VALUE_S(DESCRIPTOR_NAME, \
            "Invalid parameter: DESCRIPTOR_NAME must appear exactly once and be a valid C++ identifier") \
        , \
        NX_ARGS_RULE_UNIQUE_VALUE_S(READ, \
            "Invalid parameter: READ must appear exactly once and name a valid C++ function") \
        , \
        NX_ARGS_RULE_UNIQUE_VALUE_S(WRITE, \
            "Invalid parameter: WRITE must appear exactly once and name a valid C++ function") \
        , \
        NX_ARGS_RULE_UNIQUE_VALUE_S(NOTIFY, \
            "Invalid parameter: NOTIFY must appear exactly once and name a valid NX_SIGNAL") \
        , \
        NX_ARGS_RULE_UNIQUE_VALUE_S(RESET, \
            "Invalid parameter: RESET must appear exactly once and name a valid C++ function") \
        , \
        NX_ARGS_RULE_UNIQUE_VALUE_S(USING_READ, \
            "Invalid parameter: USING_READ must appear exactly once and reference an existing member function") \
        , \
        NX_ARGS_RULE_UNIQUE_VALUE_S(USING_WRITE, \
            "Invalid parameter: USING_WRITE must appear exactly once and reference an existing member function") \
        , \
        NX_ARGS_RULE_UNIQUE_VALUE_S(USING_NOTIFY, \
            "Invalid parameter: USING_NOTIFY must appear exactly once and reference an existing member function") \
        , \
        NX_ARGS_RULE_UNIQUE_VALUE_S(USING_RESET, \
            "Invalid parameter: USING_RESET must appear exactly once and reference an existing member function") \
        , \
        NX_ARGS_RULE_UNIQUE_VALUE_S(MEMBER, \
            "Invalid parameter: MEMBER must appear exactly once and reference an existing data member") \
        , \
        NX_ARGS_RULE_UNIQUE_VALUE_S(COMMENT, \
            "Invalid parameter: COMMENT must appear exactly once and contain a const C-string expression") \
        , \
        NX_ARGS_RULE_UNIQUE_VALUE_S(DEFAULT, \
            "Invalid parameter: DEFAULT must appear exactly once and provide a valid value for TYPE") \
        , \
        NX_ARGS_RULE_MANDATORY_S(TYPE, \
            "Missing TYPE parameter: NX_PROPERTY must have a type") \
        , \
        NX_ARGS_RULE_MANDATORY_S(NAME, \
            "Missing NAME parameter: NX_PROPERTY must have a name") \
        , \
        NX_ARGS_RULE_CONFLICT_S(READ, USING_READ, \
            "Conflicting parameters [READ, USING_READ]: exactly one getter must be specified") \
        , \
        NX_ARGS_RULE_CONFLICT_S(WRITE, USING_WRITE, \
            "Conflicting parameters [WRITE, USING_WRITE]: exactly one setter must be specified") \
        , \
        NX_ARGS_RULE_CONFLICT_S(NOTIFY, USING_NOTIFY, \
            "Conflicting parameters [NOTIFY, USING_NOTIFY]: exactly one notifier must be specified") \
        , \
        NX_ARGS_RULE_CONFLICT_S(RESET, USING_RESET, \
            "Conflicting parameters [RESET, USING_RESET]: exactly one reset function must be specified") \
        , \
        NX_ARGS_RULE_CONFLICT_S(CONST, WRITE, \
            "Conflicting parameters [CONST, WRITE]: const property cannot have a setter") \
        , \
        NX_ARGS_RULE_CONFLICT_S(CONST, USING_WRITE, \
            "Conflicting parameters [CONST, USING_WRITE]: const property cannot have a setter") \
        , \
        NX_ARGS_RULE_CONFLICT_S(CONST, RESET, \
            "Conflicting parameters [CONST, RESET]: const property cannot have a reset function") \
        , \
        NX_ARGS_RULE_CONFLICT_S(CONST, USING_RESET, \
            "Conflicting parameters [CONST, USING_RESET]: const property cannot have a reset function") \
        , \
        NX_ARGS_RULE_UNIQUE_FLAG_S(CONST, \
            "Invalid parameter: CONST must appear at most once and must not have a value") \
        , \
        NX_ARGS_RULE_UNIQUE_FLAG_S(MUTABLE, \
            "Invalid parameter: MUTABLE must appear at most once and must not have a value") \
        )


#define _nx_cxx_20_property(...) \
    _nx_args_parse( \
        _nx_cxx_20_property_parser, \
        _nx_cxx_20_property_rule_set, \
        __VA_ARGS__ \
    )


#define _nx_enable_properties(Class) \
    using _nx_property_this_type = Class;


#define NX_ENABLE_PROPERTIES(Class) \
    _nx_enable_properties(Class)

#endif
#endif //PROPERTY_DEFS_CXX20_HPP
