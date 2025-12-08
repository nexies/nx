//
// Created by nexie on 25.11.2025.
//

#ifndef PROPERTY_DEFS_HPP
#define PROPERTY_DEFS_HPP

#include "nx/core/macro/repeat.hpp"
#include "nx/core/macro/arg_count.hpp"
#include "nx/core/macro/choose.hpp"
#include "nx/core/macro/logic.hpp"

#define __NX_PROBE() ~, 1
#define __NX_PROBE_N(n) ~, n
#define __NX_IS_PROBE(...) NX_CHOOSE(1, __VA_ARGS__, 0)

#define __NX_PROPERTY_PROBE_TRAIT_TYPE __NX_PROBE_N(1)
#define __NX_PROPERTY_PROBE_TRAIT_NAME __NX_PROBE_N(2)
#define __NX_PROPERTY_PROBE_TRAIT_READ __NX_PROBE_N(3)
#define __NX_PROPERTY_PROBE_TRAIT_WRITE __NX_PROBE_N(4)

#define __NX_PROPERTY_TRAIT_ID(tok) __NX_IS_PROBE(NX_CONCAT(__NX_PROPERTY_PROBE_TRAIT_, tok))

// __NX_PROPERTY_EXTRACT_TYPE(TYPE int, NAME height) -> int
// __NX_PROPERTY_EXTRACT_TYPE(NAME height, TYPE int) -> int

// __NX_PROPERTY_EXTRACT_NAME(TYPE int, NAME height) => height

// NX_PROPERTY(TYPE int, NAME height, READ height, WRITE setHeight, NOTIFY heightChanged)
// int m_height;
// int height() const { return m_height; }
// NX_SIGNAL(heightChanged, int)
// void setHeight(int new_height) { if(m_height != new_height) heightChanged.emit(new_height); m_height = new_height; }

// __NX_PROPERTY_CREATE_DEFINITION_HELPER_1()
// __NX_PROPERTY_CREATE_READ_HELPER_1()
// __NX_PROPERTY_CREATE_NOTIFY_HELPER_0()
// __NX_PROPERTY_CREATE_WRITE_HELPER_1()


#define TYPE(x)   (TYPE, x)
#define NAME(x)   (NAME, x)
#define READ(x)   (READ, x)
#define WRITE(x)  (WRITE, x)
#define NOTIFY(x) (NOTIFY, x)

// Разворачиваем (KEY, value) в вызов __NX_PROP_TYPE_ONE_KEY(value)
#define __NX_PROP_TYPE_ONE(arg)    __NX_PROP_TYPE_ONE_I arg
#define __NX_PROP_TYPE_ONE_I(key, val) __NX_PROP_TYPE_ONE_##key(val)

// Для нужного ключа TYPE возвращаем value
#define __NX_PROP_TYPE_ONE_TYPE(t) t

// Для всех остальных ключей – ничего
#define __NX_PROP_TYPE_ONE_NAME(x)
#define __NX_PROP_TYPE_ONE_READ(x)
#define __NX_PROP_TYPE_ONE_WRITE(x)
#define __NX_PROP_TYPE_ONE_NOTIFY(x)

// Вариант на два аргумента (по твоему примеру)
// #define __NX_PROPERTY_EXTRACT_TYPE(a1, a2) \
// __NX_PROP_TYPE_ONE(a1) __NX_PROP_TYPE_ONE(a2)

#define __NX_PROPERTY_UNWRAP_COMMA_HELPER_(c, m, n, ...) m(n, __VA_ARGS__)

#define __NX_PROP_CHOOSE_TYPE_ONE_(n, ...) __NX_PROP_TYPE_ONE(NX_CHOOSE(n, __VA_ARGS__))

#define __NX_PROP_CHOOSE_TYPE_ONE(...) __NX_PROP_CHOOSE_TYPE_ONE_(__VA_ARGS__)

#define __NX_PROPERTY_EXTRACT_TYPE(...) \
    __NX_REPEAT_WITH_HELPER_(\
    NX_ARGS_COUNT(__VA_ARGS__),\
    __NX_PROPERTY_UNWRAP_COMMA_HELPER_,\
    __NX_PROP_CHOOSE_TYPE_ONE,\
    __VA_ARGS__)

#endif //PROPERTY_DEFS_HPP
