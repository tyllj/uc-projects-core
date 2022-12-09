//
// Created by tyll on 09.12.22.
//

#ifndef GARAGE_ARRAY_H
#define GARAGE_ARRAY_H

#include <stddef.h>

namespace core { namespace arrays {
    template<typename T, size_t n>
    constexpr auto length(T (& arr)[n]) -> size_t {
        return n;
    }
}}

#endif //GARAGE_ARRAY_H
