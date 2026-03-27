include(nxCMakeLogScope)

nx_push_log_scope(options)

## DOCS
if(NX_BUILD_DOCS)
    nx_log(WARNING "Build docs option is not implemented")
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

