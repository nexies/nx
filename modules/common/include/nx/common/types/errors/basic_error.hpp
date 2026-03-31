//
// Created by nexie on 29.03.2026.
//

#ifndef NX_COMMON_BASIC_ERROR_HPP
#define NX_COMMON_BASIC_ERROR_HPP
#include <string>
#include <type_traits>
#include <boost/system/error_code.hpp>
#include <pstl/glue_execution_defs.h>

#include "nx/common/helpers.hpp"


namespace nx {

    ///
    ///  class basic_error_payload {
    ///     std::string_view str () const;
    ///     int code;
    ///  }
    ///
    ///  template<typename Payload>
    ///  class basic_error {
    ///
    ///  }
    ///
    ///  using error = basic_error<basic_error_payload>;
    ///
    ///  if (err) // this is an error
    ///  {
    ///     err.code();     -> int
    ///     err.str();      -> std::string_view
    ///     err.what();     -> std::string_view (same as str)
    ///     err.where();    -> std::source_location
    ///     std::cerr << err;   -> fancy representation of what and where is broken
    ///     err.info();
    ///  }
    ///
    ///  err.expect([&] (auto err) { std::cerr << err << std::endl; })
    ///
    ///
    ///  nx::result<int> <- int or nx::error
    ///
    ///  for example:
    ///     nx::result<int> foo();
    ///
    ///     auto n = foo();
    ///     n.value();  -> int
    ///     n.error();  -> error;
    ///
    ///     if(n.error()) {
    ///        // result is error
    ///     }
    ///
    ///
    ///     nx::throw_error(nx::err::io_device_failed);
    ///
    ///     return err::io_device_failed("")
    ///
    ///     nx::user_error()
    ///     nx::generic_error();
    ///     nx::system_error();
    ///     nx::io_error();
    ///
    ///     throw err::invalid_argument("");
    ///
    ///     nx::explain(const nx::error & err);
    ///
    ///
    ///
    ///     Desired interface of class nx::error:
    ///         1) Inherits or adopts std::error_code ->
    ///             can be initialized with std::error_code or
    ///             with arguments, that initialize std::error_code;
    ///
    ///         2) Inherits std::exception and implements what() member
    ///             function for explanation with default catch
    ///
    ///         3) Allows for implementation of user-defined
    ///             error_domain class - user-defined error codes interpretation
    ///             class. error_domain class must implement its own interface, similar to
    ///             std::error_category: default explanations for error codes
    ///             predefined error_domains are:
    ///                 error_domain_system
    ///                 error_domain_io
    ///                 error_domain_user
    ///
    ///         4) Allows for initialization without specifying error_domain -
    ///             this way it is treated as user_error with specific error code
    ///             'undefined'
    ///
    ///         5) Predefined ancestor classes, inheriting nx::error are:
    ///             user_error
    ///             system_error
    ///             io_error
    ///
    ///             Each specification links directly to it's corresponding error domain
    ///             system_error and io_error can be initialized only with an integer code value
    ///
    ///             user_error can be initialized with:
    ///                 integer error code value,
    ///                 string literal
    ///                 both
    ///
    ///          6) in namespace nx::err should be implemented default
    ///           nx::error instances, corresponding to std::errc values.
    ///           for example:
    ///             static inline nx::io_error device_or_resource_busy(std::errc::device_or_resource_busy)
    ///                  ???not suse about this one???
    ///
    ///          7) Comments on error: operator() called on nx::error
    ///             returns the same error value but with a comment attached to it.
    ///             this way user can use, say, nx::invalid_argument("Argument 'count' is not allowed")
    ///
    ///          8) Throwing nx::error:
    ///             nx:error when initialized for throwing as an exception
    ///             must save source_location of the place which it was thrown as.
    ///             desired syntax:
    ///                 throw err::connection_refused("Connection is refused by the peer");
    ///
    ///
    ///


}


#endif //NX_COMMON_BASIC_ERROR_HPP
