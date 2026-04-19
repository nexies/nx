//
// Created by nexie on 4/6/2026.
//

#ifndef NX_COMMON_ERROR_CODES_HPP
#define NX_COMMON_ERROR_CODES_HPP

#include <nx/common/types/errors/error.hpp>

#define nx_error_code_1(name) \
    struct name : public nx::error { \
        explicit \
        name (std::string_view comment, const nx::source_location & loc = nx::source_location::current()) \
            : nx::error(std::make_error_code(std::errc::name), comment, loc) \
            { } \
        }; \

// // #define nx_error_code(name) \
// //     static inline auto name = nx::error(std::errc::name);

/// What do I want:
///
///     nx::error
///
///     nx::runtime_error
///         - nx::access_error
///
///     nx::logic_error
///         - nx::invalid_argument
///         - nx::invalid_instruction
///         - nx::invalid_order
///         - nx::function_not_supported
///
///     nx::access_error
///         - nx::
///
///     nx::memory_error
///
///     nx::user_error
///
///     nx::system_error ?
///
///
///     nx::json_error
///         - nx::invalid_json_key
///
///     nx::sql_error
///         - nx::invalid_query
///         -
///


# define nx_error_code(...)

namespace nx::err
{
    nx_error_code(address_family_not_supported)
    nx_error_code(address_in_use)
    nx_error_code(address_not_available)
    nx_error_code(already_connected)
    nx_error_code(argument_list_too_long)
    nx_error_code(argument_out_of_domain)
    nx_error_code(bad_address)
    nx_error_code(bad_file_descriptor)
    nx_error_code(bad_message)
    nx_error_code(broken_pipe)
    nx_error_code(connection_aborted)
    nx_error_code(connection_already_in_progress)
    nx_error_code(connection_refused)
    nx_error_code(connection_reset)
    nx_error_code(cross_device_link)
    nx_error_code(destination_address_required)
    nx_error_code(device_or_resource_busy)
    nx_error_code(directory_not_empty)
    nx_error_code(executable_format_error)
    nx_error_code(file_exists)
    nx_error_code(file_too_large)
    nx_error_code(filename_too_long)
    nx_error_code(function_not_supported)
    nx_error_code(host_unreachable)
    nx_error_code(identifier_removed)
    nx_error_code(illegal_byte_sequence)
    nx_error_code_1(inappropriate_io_control_operation)
    nx_error_code(interrupted)
    nx_error_code_1(invalid_argument)
    nx_error_code(invalid_seek)
    nx_error_code(io_error)
    nx_error_code(is_a_directory)
    nx_error_code(message_size)
    nx_error_code(network_down)
    nx_error_code(network_reset)
    nx_error_code(network_unreachable)
    nx_error_code(no_buffer_space)
    nx_error_code(no_child_process)
    nx_error_code(no_link)
    nx_error_code(no_lock_available)
    // nx_error_code(no_message_available)
    nx_error_code(no_message)
    nx_error_code(no_protocol_option)
    nx_error_code(no_space_on_device)
    // nx_error_code(no_stream_resources)
    nx_error_code(no_such_device_or_address)
    nx_error_code(no_such_device)
    nx_error_code(no_such_file_or_directory)
    nx_error_code(no_such_process)
    nx_error_code(not_a_directory)
    nx_error_code(not_a_socket)
    // nx_error_code(not_a_stream)
    nx_error_code(not_connected)
    nx_error_code(not_enough_memory)
    nx_error_code(not_supported)
    nx_error_code(operation_canceled)
    nx_error_code(operation_in_progress)
    nx_error_code(operation_not_permitted)
    nx_error_code(operation_not_supported)
    nx_error_code(operation_would_block)
    nx_error_code(owner_dead)
    nx_error_code(permission_denied)
    nx_error_code(protocol_error)
    nx_error_code(protocol_not_supported)
    nx_error_code(read_only_file_system)
    nx_error_code(resource_deadlock_would_occur)
    nx_error_code(resource_unavailable_try_again)
    nx_error_code(result_out_of_range)
    nx_error_code(state_not_recoverable)
    // nx_error_code(stream_timeout)
    nx_error_code(text_file_busy)
    nx_error_code(timed_out)
    nx_error_code(too_many_files_open_in_system)
    nx_error_code(too_many_files_open)
    nx_error_code(too_many_links)
    nx_error_code(too_many_symbolic_link_levels)
    nx_error_code(value_too_large)
    nx_error_code(wrong_protocol_type)
}

#undef nx_error_code

#endif //NX_COMMON_ERROR_CODES_HPP