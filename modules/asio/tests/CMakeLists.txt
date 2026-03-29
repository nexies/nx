add_executable(nx_asio_test_backend
        backend_test.cpp
)

target_link_libraries(nx_asio_test_backend PUBLIC
        nx::asio
)