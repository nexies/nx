if(NOT TARGET nx::network)
    nx_log("nx::tg module requires nx::network module, which is not configured - skipping...")
    return()
endif()

find_package(nlohmann_json CONFIG)

if(NOT TARGET nlohmann_json::nlohmann_json)
    nx_log("nx::tg module requires nlohmann_json, which is not configured - skipping...")
    return()
endif()

add_library(nx_tg
    src/bot.cpp
)

target_link_libraries(nx_tg PUBLIC
    nx::core2
    nx::network
    nlohmann_json::nlohmann_json
)

nx_configure_component(nx_tg
    NAME tg
    VERSION 0.1.1
)

add_subdirectory(demo)
