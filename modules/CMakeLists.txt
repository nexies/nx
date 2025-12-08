if(NOT DEFINED NX_BUILD_SHARED_LIBS)
    set(NX_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})
endif()

#if(NX_ENABLE_CXX_20_FEATURES)
#    add_compile_options(std=c++20)
#endif()

if (NX_BUILD_MODULE_CORE)
    add_subdirectory(core)
    set(NX_BUILD_MODULE_CORE2 ON)
    add_subdirectory(core2)
endif ()

if (NX_BUILD_MODULE_APP)
    add_subdirectory(app)
endif()


message("=========================================================================================================")
