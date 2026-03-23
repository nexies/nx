add_executable(nx_core_test_asio_context
        asio_context.cpp
)
target_link_libraries(nx_core_test_asio_context PUBLIC nx::core)