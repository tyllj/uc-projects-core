//
// Created by tyll on 21.09.22.
//

#ifndef SGLOGGER_CONVERT_H
#define SGLOGGER_CONVERT_H


#include <stdint.h>
#include <stdlib.h>

namespace core { namespace convert {
    inline uint8_t toUInt8(const char* str, uint8_t base = 10) {
        return static_cast<uint8_t>(strtoul(str, nullptr, base));
    }

    inline uint16_t toUInt16(const char* str, uint8_t base = 10) {
        return static_cast<uint16_t>(strtoul(str, nullptr, base));
    }

    inline uint32_t toUInt32(const char* str, uint8_t base = 10) {
        return static_cast<uint32_t>(strtoul(str, nullptr, base));
    }

    inline int8_t toInt8(const char* str, uint8_t base = 10) {
        return static_cast<int8_t>(strtol(str, nullptr, base));
    }

    inline int16_t toInt16(const char* str, uint8_t base = 10) {
        return static_cast<int16_t>(strtol(str, nullptr, base));
    }

    inline int32_t toInt32(const char* str, uint8_t base = 10) {
        return static_cast<int32_t>(strtol(str, nullptr, base));
    }
}}
#endif //SGLOGGER_CONVERT_H
