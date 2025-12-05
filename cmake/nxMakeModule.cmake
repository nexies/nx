# Настраивает уже существующую библиотечную цель как компонент пакета nx.
# Треб.: CMake >= 3.20 (File Set — с 3.23, включаем по условию).
#
# Пример:
#   nx_configure_component(
#     TARGET core
#     NAME   core
#     VERSION "${PROJECT_VERSION}"
#     PUBLIC_HEADERS
#       ${CMAKE_SOURCE_DIR}/include/nx/core/core.hpp
#     HEADERS_BASE_DIRS
#       ${CMAKE_SOURCE_DIR}/include
#     PUBLIC_DEPENDS fmt::fmt
#   )

include_guard(GLOBAL)
include(nxMakeModuleVersion)

define_property(GLOBAL PROPERTY NX_AVAILABLE_COMPONENTS BRIEF_DOCS "nx components (all)"    FULL_DOCS "All nx components declared")
define_property(GLOBAL PROPERTY NX_ENABLED_COMPONENTS   BRIEF_DOCS "nx components (built)"  FULL_DOCS "Enabled nx components")

function(nx_make_module)
    set(_one NAME TARGET VERSION)
    set(_multi
            PUBLIC_HEADERS
            HEADERS_BASE_DIRS
            PUBLIC_DEPENDS
            PRIVATE_DEPENDS
            PUBLIC_DEFINES
            PRIVATE_DEFINES
            PUBLIC_INCLUDE_DIRS
            PRIVATE_INCLUDE_DIRS
    )
    message("=========================================================================================================")

    cmake_parse_arguments(_NX "" "${_one}" "${_multi}" ${ARGN})
    set (DESC "nx_make_module()")

    if(NOT _NX_TARGET)
        message(FATAL_ERROR "${DESC}: parameter TARGET is not provided")
        return ()
    endif()
    if(NOT TARGET ${_NX_TARGET})
        message(FATAL_ERROR "${DESC}: target '${_NX_TARGET}' does not exist")
        return ()
    endif()
    if(NOT _NX_NAME)
        message("${DESC}: parameter NAME is not specified, will use target name '${_NX_TARGET}'")
        set(_NX_NAME ${_NX_TARGET})
    endif()

    string(TOUPPER "${_NX_NAME}" _name_up)
    set(_name   ${_NX_NAME})
    set(_t      ${_NX_TARGET})
    set(_alias  ${NX_NAMESPACE}::${_name})
    set(DESC "[${_alias}] -- ")

    if(NOT NX_BUILD_MODULE_${_name_up} AND NOT NX_BUILD_MODULE_${_name})
        get_property(_all GLOBAL PROPERTY NX_AVAILABLE_COMPONENTS)
        list(APPEND _all "${_name}")
        list(REMOVE_DUPLICATES _all)
        set_property(GLOBAL PROPERTY NX_AVAILABLE_COMPONENTS "${_all}")
        return()
    endif()

    message(${DESC} "Configuring component...")
    message(${DESC} "Ignoring NX_${_name_up}_BUILD_SHARED_LIBS parameter...")
    if(NOT TARGET ${_alias})
        add_library(${_alias} ALIAS ${_t})
    endif()

    target_compile_options(${_t} PUBLIC -std=c++${NX_CXX_STANDARD})

    nx_make_module_version(NAME ${_name})

    target_include_directories(${_t} PRIVATE
            $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/nx>
            $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/generated/nx>
            $<INSTALL_INTERFACE:include>
    )

    # ---------- Установка и экспорт ----------
    # ВАЖНО: если используем FILE_SET, его НУЖНО и УСТАНОВИТЬ, и ЭКСПОРТИРОВАТЬ.

    # Авто-сбор заголовков, если HEADERS_BASE_DIRS указаны, но PUBLIC_HEADERS нет
    if(_NX_HEADERS_BASE_DIRS AND NOT _NX_PUBLIC_HEADERS)
        set(_auto_headers "")
        foreach(_hdr_root IN LISTS _NX_HEADERS_BASE_DIRS)
            if(IS_DIRECTORY "${_hdr_root}")
                file(GLOB_RECURSE _found_hdrs
                        "${_hdr_root}/*.h"
                        "${_hdr_root}/*.hpp"
                        "${_hdr_root}/*.hh"
                        "${_hdr_root}/*.hxx"
                        "${_hdr_root}/*.inc"
                )
                list(APPEND _auto_headers ${_found_hdrs})
            endif()
        endforeach()

        list(REMOVE_DUPLICATES _auto_headers)

        # Заполняем PUBLIC_HEADERS, чтобы остальная логика работала без изменений
        set(_NX_PUBLIC_HEADERS ${_auto_headers})
    endif()

    set(_do_fileset OFF)
    if(_NX_PUBLIC_HEADERS AND CMAKE_VERSION VERSION_GREATER_EQUAL "3.23")
        set(_do_fileset ON)
        if(NOT _NX_HEADERS_BASE_DIRS)
            set(_NX_HEADERS_BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include")
        endif()
#        message(${DESC} "Headers base dir is set to ${_NX_HEADERS_BASE_DIRS}...")
        list(LENGTH _NX_PUBLIC_HEADERS _headers_count)
        message(${DESC} "Found ${_headers_count} public headers")
        target_sources(${_t}
                PUBLIC
                FILE_SET public_headers
                TYPE HEADERS
                BASE_DIRS ${_NX_HEADERS_BASE_DIRS}
                FILES     ${_NX_PUBLIC_HEADERS}
        )
    endif()

    if(_do_fileset)
        message(${DESC} "Installing using fileset...")
        install(TARGETS ${_t}
                EXPORT ${NX_EXPORT_SET}
                FILE_SET public_headers DESTINATION ${NX_INSTALL_INCLUDEDIR}
                ARCHIVE DESTINATION lib
                LIBRARY DESTINATION lib
                RUNTIME DESTINATION bin
                INCLUDES DESTINATION ${NX_INSTALL_INCLUDEDIR}
        )
    else()
        # Классическая установка без FILE_SET
        install(TARGETS ${_t}
                EXPORT ${NX_EXPORT_SET}
                ARCHIVE DESTINATION lib
                LIBRARY DESTINATION lib
                RUNTIME DESTINATION bin
                INCLUDES DESTINATION ${NX_INSTALL_INCLUDEDIR}
        )
        if(_NX_PUBLIC_HEADERS)
            message(${DESC} "Has public headers, installing...")
            install(FILES ${_NX_PUBLIC_HEADERS}
                    DESTINATION ${NX_INSTALL_INCLUDEDIR}
            )
        endif()
    endif()

    message(${DESC}
            "Installing public header interface: BUILD[${_NX_HEADERS_BASE_DIRS}],
                                                 INSTALL[${NX_INSTALL_INCLUDEDIR}]")
    target_include_directories(${_t}
            PUBLIC
            $<BUILD_INTERFACE:${_NX_HEADERS_BASE_DIRS}>
            $<INSTALL_INTERFACE:${NX_INSTALL_INCLUDEDIR}>
    )

    get_property(_all GLOBAL PROPERTY NX_AVAILABLE_COMPONENTS)
    list(APPEND _all "${_name}")
    list(REMOVE_DUPLICATES _all)
    set_property(GLOBAL PROPERTY NX_AVAILABLE_COMPONENTS "${_all}")

    get_property(_en GLOBAL PROPERTY NX_ENABLED_COMPONENTS)
    list(APPEND _en "${_name}")
    list(REMOVE_DUPLICATES _en)
    set_property(GLOBAL PROPERTY NX_ENABLED_COMPONENTS "${_en}")

    get_property(_ver GLOBAL PROPERTY NX_${_name_up}_VERSION_STR)
    message(${DESC} "Configured module ${_name} version ${_ver}")

endfunction()