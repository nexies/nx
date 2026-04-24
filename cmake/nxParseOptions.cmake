include(nxCMakeLogScope)

nx_push_log_scope(options)

## DOCS
if(NX_BUILD_DOCS)
    nx_log(WARNING "Build docs option is not implemented")
endif()

## TESTS
if(NX_BUILD_TESTS)
    include(CTest)

    find_package(Catch2 3 CONFIG QUIET)
    if(Catch2_FOUND)
        set(CATCH2_WITH_MAIN_LIBRARY Catch2::Catch2WithMain)
        set(CATCH2_LIBRARY Catch2::Catch2)
    endif()

    set(NX_CATCH_SUBPROJECT ${NX_PROJECT_SOURCE_DIR}/third-party/Catch2)

    if(NOT Catch2_FOUND)
        if(NX_CATCH_SUBPROJECT)
            add_subdirectory(${NX_CATCH_SUBPROJECT})
            if(TARGET Catch2::Catch2WithMain)
                set(Catch2_FOUND TRUE)
                set(CATCH2_WITH_MAIN_LIBRARY Catch2WithMain)
                set(CATCH2_LIBRARY Catch2)
            endif()
        endif()
    endif()

    if(NOT Catch2_FOUND)
        message(SEND_ERROR "Catch2 is not found and cannot be fetched. Tests will not be built.")
        set(NX_BUILD_TESTS OFF)
    endif()

endif()


## MODULES
if(NX_BUILD_MODULES)
    if(NOT DEFINED NX_MODULES_PATH)
        set(NX_MODULES_PATH ${NX_PROJECT_SOURCE_DIR}/modules)
    endif()
    nx_log("Build modules option - ON")
    add_subdirectory(${NX_MODULES_PATH})
endif()

## EXAMPLES
if(NX_BUILD_EXAMPLES)
    if(NOT DEFINED NX_EXAMPLES_PATH)
        set(NX_EXAMPLES_PATH ${NX_PROJECT_SOURCE_DIR}/examples)
    endif()

    if(NOT DEFINED NX_EXAMPLES_BINARY_DIR)
        set(NX_EXAMPLES_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/examples)
    endif()

    nx_log("Build examples - ON")
    add_subdirectory(${NX_EXAMPLES_PATH})
endif()

