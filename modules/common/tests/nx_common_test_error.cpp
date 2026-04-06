//
// Created by nexie on 4/1/2026.
//

#include <nx/common/types/errors/error.hpp>
#include <iostream>

namespace test
{
    void
    bool_operator ()
    {
        nx::error err;

        if (err)
            std::cerr << "err() is an error" << std::endl;
        else
            std::cerr << "err() is not an error" << std::endl;

        nx::error err2(std::errc::address_family_not_supported);
        if (err2)
            std::cerr << "err2() is an error" << std::endl;
        else
            std::cerr << "err2() is not an error" << std::endl;
    }

    void error_description ()
    {
        nx::error err(std::make_error_code(std::errc::address_family_not_supported),
            "Fix this dumbass", nx::source_location::current());

        std::cerr << err.what() << std::endl;
    }
}

int main (int, char * [])
{
    auto err = nx::error(std::make_error_code(std::errc::address_family_not_supported));
    err = err("Address family not supported HAHAHAHAHAHA");
    std::cerr << err.what();
    return 0;
}