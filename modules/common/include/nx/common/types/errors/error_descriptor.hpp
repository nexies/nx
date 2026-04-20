//
// Created by nexie on 31.03.2026.
//

#ifndef NX_COMMON_ERROR_DESCRIPTOR_HPP
#define NX_COMMON_ERROR_DESCRIPTOR_HPP

#include <string>
#include <nx/common/types/source_location.hpp>

namespace nx {

    class error_descriptor {
        explicit error_descriptor(const nx::source_location& location, const std::string& comment);
        error_descriptor(const error_descriptor& other);

        NX_DISABLE_MOVE(error_descriptor);
    public:
        error_descriptor();

        std::error_category* category;
        nx::source_location  location;
        std::string          comment;

        friend error_descriptor*
        make_error_descriptor(const nx::source_location& location, const std::string& comment);

        friend error_descriptor*
        make_error_descriptor(const error_descriptor& other);

        friend void
        free_error_descriptor(error_descriptor*);
    };

    NX_NODISCARD error_descriptor*
    make_error_descriptor(const nx::source_location& location, const std::string& comment);

    NX_NODISCARD error_descriptor*
    make_error_descriptor(const error_descriptor& other);

    void
    free_error_descriptor(error_descriptor*);

}

#endif //NX_COMMON_ERROR_DESCRIPTOR_HPP
