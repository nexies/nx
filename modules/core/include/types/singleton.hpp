//
// Created by nexie on 11.11.2025.
//

#ifndef SINGLETON_HPP
#define SINGLETON_HPP

namespace nx {
    template<typename T>
    class singleton {
        public:
        static T& instance() {}
    };
}

#endif //SINGLETON_HPP
