add_library(nx_asio
    include/nx/asio/backend/backend_types.hpp
    include/nx/asio/backend/backend.hpp
)

include(nxMakeModule)

nx_make_module(
        TARGET nx_asio
        NAME asio
        PUBLIC_HEADERS_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
)