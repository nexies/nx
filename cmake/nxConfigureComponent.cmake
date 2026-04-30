include_guard(GLOBAL)
include(GNUInstallDirs)

define_property(GLOBAL PROPERTY NX_AVAILABLE_COMPONENTS
    BRIEF_DOCS "nx components (all)"
    FULL_DOCS  "All nx components declared")
define_property(GLOBAL PROPERTY NX_ENABLED_COMPONENTS
    BRIEF_DOCS "nx components (built)"
    FULL_DOCS  "Enabled nx components")

# nx_configure_component(<target>
#   NAME              <name>
#   VERSION           <x.y.z>
#   PUBLIC_INCLUDE_DIR <dir>       # defaults to ${CMAKE_CURRENT_SOURCE_DIR}/include
#   PUBLIC_INCLUDES   <file> ...   # explicit header list; enables FILE_SET on CMake >= 3.23
#   NAMESPACE         <ns>         # defaults to nx
# )
#
# Sets up an existing library target as a named nx component:
#   - creates alias nx::<name>
#   - generates include/nx/<name>/version.hpp
#   - configures build/install include paths
#   - registers the target in NX_AVAILABLE_COMPONENTS / NX_ENABLED_COMPONENTS
#   - when NX_ENABLE_INSTALL=ON: installs binary, headers, and exports the target

function(nx_configure_component _target)
    set(_one   NAME VERSION PUBLIC_INCLUDE_DIR NAMESPACE)
    set(_multi PUBLIC_INCLUDES)
    cmake_parse_arguments(P "" "${_one}" "${_multi}" ${ARGN})

    # ── Validation ─────────────────────────────────────────────────────────
    if(NOT _target)
        message(FATAL_ERROR "nx_configure_component(): _target argument is required")
    endif()
    if(NOT TARGET "${_target}")
        message(FATAL_ERROR "nx_configure_component(): target '${_target}' does not exist")
    endif()

    # ── Defaults ───────────────────────────────────────────────────────────
    set(_name    "${P_NAME}")
    set(_ver     "${P_VERSION}")
    set(_pub_inc  ${P_PUBLIC_INCLUDES})
    set(_pub_inc_d "${P_PUBLIC_INCLUDE_DIR}")
    set(_nmsp    "${P_NAMESPACE}")

    if(NOT _name)
        set(_name    "${_target}")
    endif()
    if(NOT _ver)
        set(_ver     "${NX_PROJECT_VERSION}")
    endif()
    if(NOT _nmsp)
        set(_nmsp    "${NX_NAMESPACE}")
    endif()
    if(NOT _pub_inc_d)
        set(_pub_inc_d "${CMAKE_CURRENT_SOURCE_DIR}/include")
    endif()

    set(_alias "${_nmsp}::${_name}")
    string(TOUPPER "${_name}" _name_up)

    nx_push_log_scope("${_alias}")

    # ── Alias ──────────────────────────────────────────────────────────────
    if(NOT TARGET "${_alias}")
        add_library("${_alias}" ALIAS "${_target}")
    endif()

    # ── Version header ─────────────────────────────────────────────────────
    nx_make_module_version(NAME "${_name}" VERSION "${_ver}")

    # ── Include paths ──────────────────────────────────────────────────────
    target_include_directories("${_target}" PUBLIC
        $<BUILD_INTERFACE:${_pub_inc_d}>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/generated>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )

    # ── MSVC: standard-conforming preprocessor + large-object support ──────
    # /Zc:preprocessor  — required for nx macro system (__VA_ARGS__ forwarding)
    # /bigobj           — modules with many template instantiations exceed default limit
    if(MSVC)
        target_compile_options("${_target}" PUBLIC /Zc:preprocessor /bigobj)
    endif()

    # ── FILE_SET (CMake >= 3.23) ───────────────────────────────────────────
    # Attaches the public header list to the target so install() can handle
    # them automatically without a separate install(FILES ...) call.
    set(_has_fileset FALSE)
    if(_pub_inc AND CMAKE_VERSION VERSION_GREATER_EQUAL "3.23")
        set(_has_fileset TRUE)
        target_sources("${_target}"
            PUBLIC
            FILE_SET  public_headers
            TYPE      HEADERS
            BASE_DIRS "${_pub_inc_d}"
            FILES     ${_pub_inc}
        )
    endif()

    # ── Install ────────────────────────────────────────────────────────────
    if(NX_ENABLE_INSTALL)
        if(_has_fileset)
            # FILE_SET carries header paths — no separate install(FILES) needed.
            install(TARGETS "${_target}"
                EXPORT   "${NX_EXPORT_SET}"
                FILE_SET public_headers DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
                ARCHIVE  DESTINATION   "${CMAKE_INSTALL_LIBDIR}"
                LIBRARY  DESTINATION   "${CMAKE_INSTALL_LIBDIR}"
                RUNTIME  DESTINATION   "${CMAKE_INSTALL_BINDIR}"
            )
        else()
            install(TARGETS "${_target}"
                EXPORT   "${NX_EXPORT_SET}"
                ARCHIVE  DESTINATION   "${CMAKE_INSTALL_LIBDIR}"
                LIBRARY  DESTINATION   "${CMAKE_INSTALL_LIBDIR}"
                RUNTIME  DESTINATION   "${CMAKE_INSTALL_BINDIR}"
                INCLUDES DESTINATION   "${CMAKE_INSTALL_INCLUDEDIR}"
            )
            if(_pub_inc)
                install(FILES ${_pub_inc}
                    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
                )
            elseif(IS_DIRECTORY "${_pub_inc_d}")
                # No explicit header list — install the whole include directory.
                install(DIRECTORY "${_pub_inc_d}/"
                    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
                    FILES_MATCHING
                        PATTERN "*.hpp"
                        PATTERN "*.h"
                        PATTERN "*.hxx"
                )
            endif()
        endif()
    endif()

    # ── Component registry ─────────────────────────────────────────────────
    foreach(_prop NX_AVAILABLE_COMPONENTS NX_ENABLED_COMPONENTS)
        get_property(_list GLOBAL PROPERTY "${_prop}")
        list(APPEND _list "${_name}")
        list(REMOVE_DUPLICATES _list)
        set_property(GLOBAL PROPERTY "${_prop}" "${_list}")
    endforeach()

    # ── Log ────────────────────────────────────────────────────────────────
    get_property(_vstr GLOBAL PROPERTY "NX_${_name_up}_VERSION_STR")
    if(_vstr)
        nx_log("(v${_vstr})")
    else()
        nx_log("configured")
    endif()

    nx_pop_log_scope()
endfunction()
