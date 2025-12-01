if(NOT DEFINED NX_BUILD_SHARED_LIBS)
    set(NX_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})
endif()

if (NX_BUILD_MODULE_CORE)
    add_subdirectory(core)
    set(NX_BUILD_MODULE_CORE2 ON)
    add_subdirectory(core2)
endif ()

if (NX_BUILD_MODULE_APP)
    add_subdirectory(app)
endif()


message("=========================================================================================================")
