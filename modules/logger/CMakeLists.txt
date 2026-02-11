add_library(nx_logger
        include/nx/logger.hpp
        src/logger.cpp
)

find_package(spdlog)

if(spdlog_FOUND)
    message(STATUS "Found SPDLOG. Logger will be using spdlog library")
    target_link_libraries(nx_logger PUBLIC spdlog::spdlog)
    target_compile_options(nx_logger PUBLIC NX_LOGGER_USE_SPDLOG)
endif()


include(nxMakeModule)
nx_make_module(
        TARGET nx_logger
        NAME logger
        HEADERS_BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
        VERSION 0.0.1
)