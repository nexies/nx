if(NOT DEFINED NX_BUILD_SHARED_LIBS)
    set(NX_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})
endif()

add_subdirectory(common)
add_subdirectory(logging)
add_subdirectory(macro)
add_subdirectory(asio)
add_subdirectory(core)
add_subdirectory(core2)
add_subdirectory(tui)
add_subdirectory(network)

#    add_subdirectory(memory)



