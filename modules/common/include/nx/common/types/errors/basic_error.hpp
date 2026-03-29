//
// Created by nexie on 29.03.2026.
//

#ifndef NX_COMMON_BASIC_ERROR_HPP
#define NX_COMMON_BASIC_ERROR_HPP
#include <type_traits>

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

    struct error_payload {
        int code_ {};
        const char * str_ {""};
    };

    template<typename Payload>
    struct is_error_payload : public std::false_type {};

    template<>
    struct is_error_payload<error_payload> : public std::true_type {};

    template<typename Payload>
    constexpr auto is_error_payload_v = is_error_payload<Payload>::value;

    template<typename Payload>
    class basic_error {
        static_assert(is_error_payload_v<Payload>, "type is not error_payload");
    };

}

#endif //NX_COMMON_BASIC_ERROR_HPP
