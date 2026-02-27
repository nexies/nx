if(NOT DEFINED NX_BUILD_SHARED_LIBS)
    set(NX_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})
endif()

if (NX_BUILD_MODULE_MEMORY)
    add_subdirectory(memory)
endif()

if (NX_BUILD_MODULE_MACRO)
    add_subdirectory(macro)
endif()

if (NX_BUILD_MODULE_LOGGER)
    add_subdirectory(logger)
endif()

if (NX_BUILD_MODULE_CORE)
    add_subdirectory(core)
endif ()

if (NX_BUILD_MODULE_IO)
    add_subdirectory(io)
endif()


#message("=========================================================================================================")
