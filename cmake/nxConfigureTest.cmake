include_guard(GLOBAL)

function(nx_configure_test NAME)
    set(_arg_opt LINK_PUBLIC NO_CATCH)
    set(_arg_one COMPONENT BINARY_NAME TARGET BINARY_DIR)
    set(_arg_many SOURCE)
    cmake_parse_arguments(PARSE_ARGV 0 _arg "${_arg_opt}" "${_arg_one}" "${_arg_many}")
# ----------------------------------------------------------------------------------------
    if(NOT NX_BUILD_TESTS)
        return()
    endif()

    set(_test_name "${NAME}")

    if(_arg_COMPONENT)
        set(_component ${_arg_COMPONENT})
        set(_test_name "${_arg_COMPONENT}_${_test_name}")
    endif()

    set(_test_name "${NX_NAMESPACE}_test_${_test_name}")
    if(_arg_BINARY_NAME)
        set(_binary ${_arg_BINARY_NAME})
    else()
        set(_binary ${_test_name})
    endif()

    if(_arg_TARGET)
        set(_target ${_arg_TARGET})
    else()
        set(_target ${_test_name})
    endif()

    if(NOT _arg_SOURCE)
        set(_sources "${_test_name}.cpp")
    else()
        set(_sources ${_arg_SOURCE})
    endif()

    if(_arg_LINK_PUBLIC)
        set(_scope PUBLIC)
    else()
        set(_scope PRIVATE)
    endif()

    if(_arg_BINARY_DIR)
        set(_binary_dir ${_arg_BINARY_DIR})
    else()
        set(_binary_dir "${CMAKE_BINARY_DIR}/tests")
        if(_component)
            set(_binary_dir "${_binary_dir}/${_component}")
        endif()
    endif()

    if(_component)
        set(_link_libraries "${NX_NAMESPACE}_${_component}")
    else()
        set(_link_libraries "")
    endif()

    if(_arg_NO_CATCH)
        set(_catch FALSE)
    else()
        if(NOT Catch2_FOUND)
            message(WARNING "[nx_configure_test] -- Catch2 is not found. Target `${_target}` will not be built")
            return()
        endif()

        set(_catch TRUE)
#        set(_link_libraries "${_link_libraries} Catch2WithMain")
    endif()


# ----------------------------------------------------------------------------------------------
    add_executable(${_target} ${_sources})

    if(_catch)
        include(Catch)
        catch_discover_tests(${_target})
        target_link_libraries(${_target} PRIVATE ${CATCH2_WITH_MAIN_LIBRARY})

    endif()

    target_link_libraries(${_target} ${_scope} ${_link_libraries})

    set_target_properties(${_target} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${_binary_dir}
            OUTPUT_NAME "${_binary}"
    )

    set_property(GLOBAL APPEND PROPERTY NX_ALL_TEST_TARGETS ${_target})

#    get_target_property(_test_incs ${_target} INCLUDE_DIRECTORIES)
#    get_target_property(_test_iincs ${_target} INTERFACE_INCLUDE_DIRECTORIES)
#    get_target_property(_lib_iincs nx_${_component} INTERFACE_INCLUDE_DIRECTORIES)
#
#    message(STATUS "test INCLUDE_DIRECTORIES = ${_test_incs}")
#    message(STATUS "test INTERFACE_INCLUDE_DIRECTORIES = ${_test_iincs}")
#    message(STATUS "nx_asio INTERFACE_INCLUDE_DIRECTORIES = ${_lib_iincs}")

endfunction()