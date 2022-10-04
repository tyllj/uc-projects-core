//
// Created by tyll on 21.09.22.
//

#ifndef UC_CORE_CONVERT_H
#define UC_CORE_CONVERT_H


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

    template<typename T>
    inline StringBuilder toString(T value) {
        return StringBuilder().append(value);
    }

    template<typename T>
    inline void toString(T value, char * buffer, size_t size) {
        StringBuilder(buffer, size).append(value);
    }

    template<typename T, size_t destinationSize>
    inline void toString(T value, char (&destination)[destinationSize]) {
        StringBuilder(destination, destinationSize).append(value);
    }

    inline void toHexString(uint8_t* data, size_t n, char* destination, size_t destinationSize) {
        StringBuilder sb(destination, destinationSize);
        for (size_t i = 0; i < n; i++)
            sb.appendHex(data[i]);
    }

    template<size_t destinationSize>
    inline void toHexString(uint8_t* data, size_t n, char (&destination)[destinationSize]) {
        toHexString(data, n, destination, destinationSize);
    }

    inline size_t fromHexString(const char *hexString, uint8_t* bytes, size_t n) {
        size_t i = 0;
        char currentByteString[3] = { 0x00 };

        while (hexString[2 * i] && hexString[2 * i + 1] && i < n) {
            currentByteString[0] = hexString[2 * i];
            currentByteString[1] = hexString[2 * i + 1];
            bytes[i++] = convert::toUInt8(currentByteString, 16);
        }
        return i;
    }

    template<size_t n>
    inline size_t fromHexString(const char* hexString, uint8_t (&bytes)[n]) {
        return fromHexString(hexString, bytes, n);
    }

}}
#endif //UC_CORE_CONVERT_H
