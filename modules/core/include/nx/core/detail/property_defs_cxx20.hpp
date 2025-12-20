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
///                 * conflicts with DEFAULT parameter
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

// # TOKENS
#include <../../../../../macro/include/nx/macro/util/arguments.hpp>

// # PARAMETERS
#define TYPE                    1
#define NAME                    2
#define DESCRIPTOR_TYPE         3
#define DESCRIPTOR_NAME         4
#define READ                    5
#define WRITE                   6
#define NOTIFY                  7
#define RESET                   8
#define USING_READ              9
#define USING_WRITE             10
#define USING_NOTIFY            11
#define MEMBER                  12

#define DEFAULT                 13
#define DEFAULTS                13  // alias

#define CONSTANT                14
#define CONST                   14  // alias

#define MUTABLE                 15
#define FINAL                   16



#endif
#endif //PROPERTY_DEFS_CXX20_HPP
