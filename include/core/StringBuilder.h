//
// Created by tyll on 11.02.22.
//

#ifndef FIRMWARE_STRINGBUILDER_H
#define FIRMWARE_STRINGBUILDER_H

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "core/CStrings.h"
#include "itoa.h"

namespace core {
    inline char _stringBuilderBuffer[255];
    class StringBuilder {
    public:
        StringBuilder() : StringBuilder(_stringBuilderBuffer, sizeof(_stringBuilderBuffer)) {}
        StringBuilder(char* str, size_t length) : _buffer(str), _length(length), _position(0) {}
        size_t length() const { return _position; }

        operator const char*() const { return toString(); }

        void append(const char* str) {
            size_t i = 0;
            while (_position < _length && str[i] != '\0') {
                _buffer[_position] = str[i];
                i++;
                _position++;
            }
            _buffer[_position] = '\0';
        }
        void append(char c) {
            if (_position >= _length)
                return;

            *ptr() = c;
            seek(1);
        }

        void append(float value, int8_t width, uint8_t decimalPlaces)  {
            append((double) value, width, decimalPlaces);
        }
        void append(double value, int8_t width, uint8_t decimalPlaces)  {
            if (_position + width >= _length)
                return;

            dtostrf(value, width, decimalPlaces, &_buffer[_position]);
            *(ptr() + _length) = '\0';
            _position += width;
        }

        void append(int32_t value, int8_t width) {
            if (_position + width >= _length)
                return;
            uint8_t intWidth = decimalLength(value);
            if (intWidth > width)
                return;
            pad(width - intWidth);
            ltoa(value, ptr(), 10);
            seek(intWidth);
        }
        void append(int32_t value) {
            uint8_t intWidth = decimalLength(value);
            if (_position + intWidth >= _length)
                return;

            ltoa(value, ptr(), 10);
            seek(intWidth);
        }
        void appendHex(int32_t value)  {
            size_t width = 8;
            if (_position + width >= _length)
                return;

            ltoa(value, ptr(), 16);
            rightAlign(ptr(), width);
            seek(strlen(ptr()));
        }

        void append(int16_t value, int8_t width) {
            if (_position + width >= _length)
                return;
            uint8_t intWidth = decimalLength(value);
            if (intWidth > width)
                return;
            pad(width - intWidth);
            itoa(value, ptr(), 10);
            seek(intWidth);
        }
        void append(int16_t value) {
            uint8_t intWidth = decimalLength(value);
            if (_position + intWidth >= _length)
                return;

            itoa(value, ptr(), 10);
            seek(strlen(ptr()));
        }
        void appendHex(int16_t value) {
            size_t width = 4;
            if (_position + width >= _length)
                return;

            ltoa(value, ptr(), 16);
            rightAlign(ptr(), width);
            seek(strlen(ptr()));
        }

        void append(int8_t value, int8_t width) {
            if (_position + width >= _length)
                return;
            uint8_t intWidth = decimalLength(value);
            if (intWidth > width)
                return;
            pad(width - intWidth);
            itoa((int16_t)value, ptr(), 10);
            seek(intWidth);
        }
        void append(int8_t value) {
            uint8_t intWidth = decimalLength(value);
            if (_position + intWidth >= _length)
                return;

            itoa((int16_t)value, ptr(), 10);
            seek(strlen(ptr()));
        }
        void appendHex(int8_t value) {
            size_t width = 2;
            if (_position + width >= _length)
                return;

            itoa(value, ptr(), 16);
            rightAlign(ptr(), width);
            seek(strlen(ptr()));
        }

        void append(uint32_t value, int8_t width) {
            if (_position + width >= _length)
                return;
            uint8_t intWidth = decimalLength(value);
            if (intWidth > width)
                return;
            pad(width - intWidth);
            ultoa(value, ptr(), 10);
            seek(intWidth);
        }
        void append(uint32_t value) {
            uint8_t intWidth = decimalLength(value);
            if (_position + intWidth >= _length)
                return;

            ultoa(value, ptr(), 10);
            seek(strlen(ptr()));
        }
        void appendHex(uint32_t value) {
            size_t width = 8;
            if (_position + width >= _length)
                return;

            ultoa(value, ptr(), 16);
            rightAlign(ptr(), width);
            seek(strlen(ptr()));
        }

        void append(uint16_t value, int8_t width) {
            if (_position + width >= _length)
                return;
            uint8_t intWidth = decimalLength(value);
            if (intWidth > width)
                return;
            pad(width - intWidth);
            utoa((int16_t)value, ptr(), 10);
            seek(intWidth);
        }
        void append(uint16_t value) {
            uint8_t intWidth = decimalLength(value);
            if (_position + intWidth >= _length)
                return;

            utoa(value, ptr(), 10);
            seek(strlen(ptr()));
        }
        void appendHex(uint16_t value) {
            size_t width = 4;
            if (_position + width >= _length)
                return;

            ultoa(value, ptr(), 16);
            rightAlign(ptr(), width);
            seek(strlen(ptr()));
        }

        void append(uint8_t value, int8_t width) {
            if (_position + width >= _length)
                return;
            uint8_t intWidth = decimalLength(value);
            if (intWidth > width)
                return;
            pad(width - intWidth);
            itoa((uint16_t)value, ptr(), 10);
            seek(intWidth);
        }
        void append(uint8_t value) {
            uint8_t intWidth = decimalLength(value);
            if (_position + intWidth >= _length)
                return;

            utoa((uint16_t)value, ptr(), 10);
            seek(strlen(ptr()));
        }
        void appendHex(uint8_t value) {
            size_t width = 2;
            if (_position + width >= _length)
                return;

            ultoa((uint16_t)value, ptr(), 16);
            rightAlign(ptr(), width);
            seek(strlen(ptr()));
        }

        uint8_t decimalLength(uint8_t value) {
            if (value < 10)
                return 1;
            if (value < 100)
                return 2;
            return 3;
        }
        uint8_t decimalLength(uint16_t value) {
            if (value < 10)
                return 1;
            if (value < 100)
                return 2;
            if (value < 1000)
                return 3;
            if (value < 10000)
                return 4;
            return 5;
        }
        uint8_t decimalLength(uint32_t value) {
            if (value <= UINT16_MAX)
                return decimalLength((uint16_t) value);
            uint8_t length = 4;
            for (uint32_t i = 10000; i <= value; i *= 10) {
                length++;
            }
            return length;
        }

        uint8_t decimalLength(int8_t value) {
            if (value <= -100)
                return 4;
            if (value <= -10)
                return 3;
            if (value < 0)
                return 2;
            if (value < 10)
                return 1;
            if (value < 100)
                return 2;
            return 3;
        }
        uint8_t decimalLength(int16_t value) {
            if (value <= -10000)
                return 6;
            if (value <= -1000)
                return 5;
            if (value <= -100)
                return 4;
            if (value <= -10)
                return 3;
            if (value < 0)
                return 2;
            if (value < 10)
                return 1;
            if (value < 100)
                return 2;
            if (value < 1000)
                return 3;
            if (value < 10000)
                return 4;
            return 5;
        }
        uint8_t decimalLength(int32_t value) {
            if (value <= INT16_MAX && value >= INT16_MIN)
                return decimalLength((int16_t) value);
            if (value < 0) {
                uint8_t length = 0;
                for (uint32_t i = 1; i <= value; i *= 10) {
                    length++;
                }
                return length;
            } else {
                uint8_t length = 1;
                for (uint32_t i = -1; i >= value; i *= 10) {
                    length++;
                }
                return length;
            }
        }

        void lineBreak() {
            append(core::cstrings::newLine(cstrings::POSIX));
        }

        void lineBreak(core::cstrings::NewLineMode mode) {
            append(mode);
        }

        const char* toString() const { return _buffer;}

    private:
        inline char* ptr() const { return &_buffer[_position]; }

        inline void seek(size_t width) {
            _position += width;
        }

        inline void pad(size_t width) {
            memset(ptr(), ' ', width);
            seek(width);
        }

        void rightAlign(char* str, uint8_t places) {
            size_t length = strlen(str);
            int16_t offset = places - length;
            if (offset < 0)
                return;

            for (int i = length; i >= 0; i--)
                str[i + offset] = str[i];

            for (int i = offset - 1; i >= 0; i--)
                str[i] = ' ';

            str[places] = '\0';
        }
        char* _buffer;
        size_t _position;
        size_t _length;
    };
}
#endif //FIRMWARE_STRINGBUILDER_H
