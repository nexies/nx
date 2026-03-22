add_executable(nx_test_func_asio_context
    asio_context.cpp
)
target_link_libraries(nx_test_func_asio_context PUBLIC nx::core)