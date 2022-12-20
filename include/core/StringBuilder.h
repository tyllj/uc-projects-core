//
// Created by tyll on 11.02.22.
//

#ifndef UC_CORE_STRINGBUILDER_H
#define UC_CORE_STRINGBUILDER_H

#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "core/CStrings.h"
#include "core/itoa.h"

namespace core {
    inline thread_local char _stringBuilderBuffer[256];
    class StringBuilder {
    public:
        StringBuilder() : StringBuilder(_stringBuilderBuffer, sizeof(_stringBuilderBuffer)) {}

        StringBuilder(char* str, size_t length) : _buffer(str), _size(length), _position(0) {}
        template<size_t n>
        StringBuilder(char (&str)[n]) : StringBuilder(str, n) {}
        auto length() const -> size_t { return _position; }

        auto append(const char* str) -> StringBuilder& {
            size_t i = 0;
            while (_position < (_size - 1) && str[i] != '\0') {
                *end() = str[i];
                i++;
                seek(1);
            }
            *end() = '\0';
            return *this;
        }

        auto append(const char* str, uint32_t count) -> StringBuilder& {
            size_t i = 0;
            while (_position < (_size - 1) && _position < count && str[i] != '\0') {
                *end() = str[i];
                i++;
                seek(1);
            }
            *end() = '\0';
            return *this;
        }

        auto append(char c) -> StringBuilder& {
            if (_position >= (_size - 1))
                return *this;

            *end() = c;
            seek(1);
            *end() = '\0';
            return *this;
        }

        auto append(float value, int8_t minWidth, uint8_t decimalPlaces) -> StringBuilder& {
            return append(static_cast<double>(value), minWidth, decimalPlaces);
        }

        auto append(double value, int8_t minWidth, uint8_t decimalPlaces) -> StringBuilder& {
            uint8_t width = decimalLength(static_cast<int32_t>(value)) + decimalPlaces;
            width += value < 0.0 ? 2 : 1; // decimal point and minus sign.
            width = max(width, static_cast<uint8_t>(abs(minWidth)));
            if (_position + width >= _size)
                return *this;

            core::dtostrf(value, minWidth, decimalPlaces, &_buffer[_position]);
            seek(cstrings::length(end()));
            return *this;
        }

        auto append(float value) -> StringBuilder& {
            return append(static_cast<double>(value));
        }

        auto append(double value) -> StringBuilder& {
            uint8_t width = decimalLength(static_cast<int32_t>(value)) + 2;
            width += value < 0.0 ? 2 : 1;
            uint8_t decimalPlaces = width == 3 ? 2 : 1;
            return append(value, width, decimalPlaces);
        }

        auto append(int32_t value, int8_t width, char padding = ' ') -> StringBuilder& {
            if (_position + width >= _size)
                return *this;
            uint8_t intWidth = decimalLength(value);
            if (intWidth > width)
                return *this;
            pad(width - intWidth, padding);
            core::ltoa(value, end(), 10);
            seek(intWidth);
            return *this;
        }

        auto append(int32_t value) -> StringBuilder& {
            uint8_t intWidth = decimalLength(value);
            if (_position + intWidth >= _size)
                return *this;

            core::ltoa(value, end(), 10);
            seek(intWidth);
            return *this;
        }

        auto appendHex(int32_t value) -> StringBuilder& {
            size_t width = 8;
            if (_position + width >= _size)
                return *this;

            core::ltoa(value, end(), 16);
            rightAlign(end(), width);
            seek(strlen(end()));
            return *this;
        }

        auto append(int16_t value, int8_t width, char padding = ' ') -> StringBuilder& {
            if (_position + width >= (_size - 1))
                return *this;
            uint8_t intWidth = decimalLength(value);
            if (intWidth > width)
                return *this;
            pad(width - intWidth, padding);
            core::itoa(value, end(), 10);
            seek(intWidth);
            return *this;
        }

        auto append(int16_t value) -> StringBuilder& {
            uint8_t intWidth = decimalLength(value);
            if (_position + intWidth >= (_size - 1))
                return *this;

            core::itoa(value, end(), 10);
            seek(strlen(end()));
            return *this;
        }

        auto appendHex(int16_t value) -> StringBuilder& {
            size_t width = 4;
            if (_position + width >= (_size - 1))
                return *this;

            core::ltoa(value, end(), 16);
            rightAlign(end(), width);
            seek(strlen(end()));
            return *this;
        }

        auto append(int8_t value, int8_t width, char padding = ' ') -> StringBuilder&{
            if (_position + width >= (_size - 1))
                return *this;
            uint8_t intWidth = decimalLength(value);
            if (intWidth > width)
                return *this;
            pad(width - intWidth, padding);
            core::itoa((int16_t)value, end(), 10);
            seek(intWidth);
            return *this;
        }

        auto append(int8_t value) -> StringBuilder& {
            uint8_t intWidth = decimalLength(value);
            if (_position + intWidth >= (_size - 1))
                return *this;

            core::itoa((int16_t)value, end(), 10);
            seek(strlen(end()));
            return *this;
        }
        auto appendHex(int8_t value) -> StringBuilder& {
            size_t width = 2;
            if (_position + width >= (_size - 1))
                return *this;

            core::itoa(value, end(), 16);
            rightAlign(end(), width);
            seek(strlen(end()));
            return *this;
        }

        auto append(uint32_t value, int8_t width, char padding = ' ') -> StringBuilder& {
            if (_position + width >= (_size - 1))
                return *this;
            uint8_t intWidth = decimalLength(value);
            if (intWidth > width)
                return *this;
            pad(width - intWidth, padding);
            core::ultoa(value, end(), 10);
            seek(intWidth);
            return *this;
        }
        auto append(uint32_t value) -> StringBuilder& {
            uint8_t intWidth = decimalLength(value);
            if (_position + intWidth >= (_size - 1))
                return *this;

            core::ultoa(value, end(), 10);
            seek(strlen(end()));
            return *this;
        }
        auto appendHex(uint32_t value) -> StringBuilder& {
            size_t width = 8;
            if (_position + width >= (_size - 1))
                return *this;

            core::ultoa(value, end(), 16);
            rightAlign(end(), width);
            seek(strlen(end()));
            return *this;
        }

        auto append(uint16_t value, int8_t width, char padding = ' ') -> StringBuilder& {
            if (_position + width >= (_size - 1))
                return *this;
            uint8_t intWidth = decimalLength(value);
            if (intWidth > width)
                return *this;
            pad(width - intWidth, padding);
            core::ultoa((int16_t)value, end(), 10);
            seek(intWidth);
            return *this;
        }

        auto append(uint16_t value) -> StringBuilder& {
            uint8_t intWidth = decimalLength(value);
            if (_position + intWidth >= (_size - 1))
                return *this;

            core::ultoa(value, end(), 10);
            seek(strlen(end()));
            return *this;
        }

        auto appendHex(uint16_t value) -> StringBuilder& {
            size_t width = 4;
            if (_position + width >= (_size - 1))
                return *this;

            core::ultoa(value, end(), 16);
            rightAlign(end(), width);
            seek(strlen(end()));
            return *this;
        }

        auto append(uint8_t value, int8_t width, char padding = ' ') -> StringBuilder& {
            if (_position + width >= (_size - 1))
                return *this;
            uint8_t intWidth = decimalLength(value);
            if (intWidth > width)
                return *this;
            pad(width - intWidth, padding);
            core::itoa((uint16_t)value, end(), 10);
            seek(intWidth);
            return *this;
        }

        auto append(uint8_t value) -> StringBuilder& {
            uint8_t intWidth = decimalLength(value);
            if (_position + intWidth >= (_size - 1))
                return *this;

            core::ultoa((uint16_t)value, end(), 10);
            seek(strlen(end()));
            return *this;
        }

        auto appendHex(uint8_t value) -> StringBuilder& {
            size_t width = 2;
            if (_position + width >= (_size - 1))
                return *this;

            core::ultoa((uint16_t)value, end(), 16);
            rightAlign(end(), width);
            seek(strlen(end()));
            return *this;
        }

        auto lineBreak() -> StringBuilder& {
            return append(::core::cstrings::newLine(cstrings::LF));
        }

        auto lineBreak(::core::cstrings::NewLineMode mode) -> StringBuilder& {
            return append(::core::cstrings::newLine(mode));
        }

        template<typename T>
        auto operator+(T var) -> StringBuilder& {
            return append(var);
        }

        template<typename T>
        auto operator+=(T var) -> StringBuilder& {
            return append(var);
        }

        auto clear() -> void {
			_position = 0;
			*end() = '\0';
        }

        auto toString(char * buffer, size_t n) -> void {
            strncpy(buffer, _buffer, n - 1);
            buffer[n] = 0;
        }

        template<size_t n>
        auto toString(char (&buffer)[n]) const -> void {
            toString(buffer, n);
        }

        auto toString() const -> core::CString {
            return { _buffer };
        }

        operator const char*() const { return _buffer; }

        char* begin() const { return &_buffer[0]; }

        char* end() const { return &_buffer[_position]; }

    private:

        void seek(size_t width) {
            _position += width;
        }

        void pad(size_t width, char padding) {
            memset(end(), padding, width);
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

        static auto decimalLength(uint8_t value) -> uint8_t {
            if (value < 10)
                return 1;
            if (value < 100)
                return 2;
            return 3;
        }

        static auto decimalLength(uint16_t value) -> uint8_t {
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

        static auto decimalLength(uint32_t value) -> uint8_t {
            if (value <= UINT16_MAX)
                return decimalLength((uint16_t) value);
            uint8_t length = 4;
            for (uint32_t i = 10000; i <= value; i *= 10) {
                length++;
            }
            return length;
        }

        static auto decimalLength(int8_t value) -> uint8_t {
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

        static auto decimalLength(int16_t value) -> uint8_t {
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

        static auto decimalLength(int32_t value) -> uint8_t {
            if (value <= INT16_MAX && value >= INT16_MIN)
                return decimalLength((int16_t) value);
            if (value >= 0) {
                uint8_t length = 0;
                for (int32_t i = 1; i <= value; i *= 10) {
                    length++;
                }
                return length;
            } else {
                uint8_t length = 1;
                for (int32_t i = -1; i >= value; i *= 10) {
                    length++;
                }
                return length;
            }
        }

        char* _buffer;
        size_t _size;
        size_t _position;
    };
}

#endif //FIRMWARE_STRINGBUILDER_H
