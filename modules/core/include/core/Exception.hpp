//
// Created by nexie on 12.11.2025.
//

#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP
#include <exception>

namespace nx
{
    class Exception : public std::exception
    {
        const char * _str;
    public:
        explicit Exception (const char * str) : _str(str) {}
        [[nodiscard]]
        const char * what () const override { return _str; }
    };
}

#endif //EXCEPTION_HPP
