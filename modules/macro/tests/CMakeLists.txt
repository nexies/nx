#include(CTest)

# Попробуем найти Catch2 (например, установленный через пакетный менеджер)
find_package(Catch2 3 CONFIG QUIET)

# Если не нашли — скачиваем через CPM/FETCHContent (минимальный вариант)
if(NOT Catch2_FOUND)
    include(FetchContent)
    FetchContent_Declare(
            catch2
            GIT_REPOSITORY https://github.com/catchorg/Catch2.git
            GIT_TAG v3.6.0 # зафиксируй нужную версию
    )
    FetchContent_MakeAvailable(catch2)
endif()

# Тест для core
#add_executable(nx_core_tests
#)

include(Catch)

#target_link_libraries(nx_core_tests
#        PRIVATE
#        nx::core
#        Catch2::Catch2WithMain
#)
#catch_discover_tests(nx_core_tests)

add_executable(nx_macro_tests
        macro-tests/main.test.cpp
        macro-tests/tuple.test.cpp
        macro-tests/limits.test.cpp
        ../../core/tests/asio_context.cpp
)
catch_discover_tests(nx_macro_tests)

target_link_libraries(nx_macro_tests PUBLIC
        nx::macro
        Catch2::Catch2WithMain
)

# (необязательно) санитайзеры и строгие предупреждения для тестов
#if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
#    target_compile_options(nx_core_tests PRIVATE -Wall -Wextra -Wpedantic)
#    option(NX_TESTS_ASAN "Enable ASAN for tests" OFF)
#    if(NX_TESTS_ASAN)
#        target_compile_options(nx_core_tests PRIVATE -fsanitize=address)
#        target_link_options(nx_core_tests PRIVATE -fsanitize=address)
#    endif()
#endif()