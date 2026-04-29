//
// Created by nexie on 4/29/2026.
//

#ifndef NX_CORE2_EVENT_HPP
#define NX_CORE2_EVENT_HPP
#include "nx/common.hpp"

namespace nx::core
{
    class event
    {
    public:
        using type_id = std::uint16_t;
    private:
        type_id type_id_;
        bool    is_accepted_;

    public:
        explicit constexpr
        event (type_id id)
            : type_id_{ id }
            , is_accepted_ { false }
        {}

        constexpr void
        accept() noexcept { is_accepted_ = true; }

        constexpr void
        ignore() noexcept { is_accepted_ = false; }

        NX_NODISCARD constexpr bool
        is_accepted() const noexcept { return is_accepted_; }

        NX_NODISCARD constexpr type_id
        type () const noexcept { return type_id_; }
    };

}

#endif //NX_CORE2_EVENT_HPP
