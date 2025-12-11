if(NOT DEFINED NX_BUILD_SHARED_LIBS)
    set(NX_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})
endif()

if (NX_BUILD_MODULE_CORE)
    set(NX_BUILD_MODULE_MACRO ON)
    add_subdirectory(macro)

    add_subdirectory(core)
endif ()

if (NX_BUILD_MODULE_APP)
    add_subdirectory(app)
endif()


message("=========================================================================================================")
