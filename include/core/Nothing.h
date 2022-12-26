//
// Created by tu.jungke on 19.12.2022.
//

#ifndef __NOTHING_HPP__
#define __NOTHING_HPP__
namespace core {
    struct Nothing {
        constexpr Nothing() = default;
    };

    constexpr Nothing nothing;
}
#endif //__NOTHING_HPP__
