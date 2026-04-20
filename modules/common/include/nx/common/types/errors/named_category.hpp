//
// Created by nexie on 4/20/2026.
//

#ifndef NX_COMMON_NAMED_CATEGORY_HPP
#define NX_COMMON_NAMED_CATEGORY_HPP

#include <system_error>

namespace nx {

    // Lightweight error_category that uses only a name.
    // description() returns empty string — message comes from error::comment().
    class named_error_category final : public std::error_category {
        const char* name_;
    public:
        explicit named_error_category(const char* name) noexcept
            : name_(name) {}

        const char* name() const noexcept override { return name_; }

        std::string message(int) const override { return {}; }
    };

}

#endif //NX_COMMON_NAMED_CATEGORY_HPP
