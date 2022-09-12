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
    inline char _stringBuilderBuffer[255];
    class StringBuilder {
    public:
        StringBuilder() : StringBuilder(_stringBuilderBuffer, sizeof(_stringBuilderBuffer)) {}
        StringBuilder(char* str, size_t length) : _buffer(str), _length(length), _position(0) {}
        size_t length() const { return _position; }

        operator const char*() const { return toCString(); }

        StringBuilder& append(const char* str) {
            size_t i = 0;
            while (_position < _length && str[i] != '\0') {
                _buffer[_position] = str[i];
                i++;
                _position++;
            }
            _buffer[_position] = '\0';
            return *this;
        }

        StringBuilder& append(const char* str, uint32_t count) {
            size_t i = 0;
            while (_position < _length && _position < count && str[i] != '\0') {
                _buffer[_position] = str[i];
                i++;
                _position++;
            }
            _buffer[_position] = '\0';
            return *this;
        }

        StringBuilder& append(char c) {
            if (_position >= _length)
                return *this;

            *ptr() = c;
            seek(1);
            return *this;
        }

        StringBuilder& append(float value, int8_t width, uint8_t decimalPlaces)  {
            append((double) value, width, decimalPlaces);
            return *this;
        }
        StringBuilder& append(double value, int8_t width, uint8_t decimalPlaces)  {
            if (_position + width >= _length)
                return *this;

            core::dtostrf(value, width, decimalPlaces, &_buffer[_position]);
            *(ptr() + _length) = '\0';
            _position += width;
            return *this;
        }

        StringBuilder& append(int32_t value, int8_t width, char padding = ' ') {
            if (_position + width >= _length)
                return *this;
            uint8_t intWidth = decimalLength(value);
            if (intWidth > width)
                return *this;
            pad(width - intWidth, padding);
            core::ltoa(value, ptr(), 10);
            seek(intWidth);
        }
        StringBuilder& append(int32_t value) {
            uint8_t intWidth = decimalLength(value);
            if (_position + intWidth >= _length)
                return *this;

            core::ltoa(value, ptr(), 10);
            seek(intWidth);
            return *this;
        }
        StringBuilder& appendHex(int32_t value)  {
            size_t width = 8;
            if (_position + width >= _length)
                return *this;

            core::ltoa(value, ptr(), 16);
            rightAlign(ptr(), width);
            seek(strlen(ptr()));
            return *this;
        }

        StringBuilder& append(int16_t value, int8_t width, char padding = ' ') {
            if (_position + width >= _length)
                return *this;
            uint8_t intWidth = decimalLength(value);
            if (intWidth > width)
                return *this;
            pad(width - intWidth, padding);
            core::itoa(value, ptr(), 10);
            seek(intWidth);
            return *this;
        }
        StringBuilder& append(int16_t value) {
            uint8_t intWidth = decimalLength(value);
            if (_position + intWidth >= _length)
                return *this;

            core::itoa(value, ptr(), 10);
            seek(strlen(ptr()));
            return *this;
        }
        StringBuilder& appendHex(int16_t value) {
            size_t width = 4;
            if (_position + width >= _length)
                return *this;

            core::ltoa(value, ptr(), 16);
            rightAlign(ptr(), width);
            seek(strlen(ptr()));
            return *this;
        }

        StringBuilder& append(int8_t value, int8_t width, char padding = ' ') {
            if (_position + width >= _length)
                return *this;
            uint8_t intWidth = decimalLength(value);
            if (intWidth > width)
                return *this;
            pad(width - intWidth, padding);
            core::itoa((int16_t)value, ptr(), 10);
            seek(intWidth);
            return *this;
        }
        StringBuilder& append(int8_t value) {
            uint8_t intWidth = decimalLength(value);
            if (_position + intWidth >= _length)
                return *this;

            core::itoa((int16_t)value, ptr(), 10);
            seek(strlen(ptr()));
            return *this;
        }
        StringBuilder& appendHex(int8_t value) {
            size_t width = 2;
            if (_position + width >= _length)
                return *this;

            core::itoa(value, ptr(), 16);
            rightAlign(ptr(), width);
            seek(strlen(ptr()));
            return *this;
        }

        StringBuilder& append(uint32_t value, int8_t width, char padding = ' ') {
            if (_position + width >= _length)
                return *this;
            uint8_t intWidth = decimalLength(value);
            if (intWidth > width)
                return *this;
            pad(width - intWidth, padding);
            core::ultoa(value, ptr(), 10);
            seek(intWidth);
            return *this;
        }
        StringBuilder& append(uint32_t value) {
            uint8_t intWidth = decimalLength(value);
            if (_position + intWidth >= _length)
                return *this;

            core::ultoa(value, ptr(), 10);
            seek(strlen(ptr()));
            return *this;
        }
        StringBuilder& appendHex(uint32_t value) {
            size_t width = 8;
            if (_position + width >= _length)
                return *this;

            core::ultoa(value, ptr(), 16);
            rightAlign(ptr(), width);
            seek(strlen(ptr()));
            return *this;
        }

        StringBuilder& append(uint16_t value, int8_t width, char padding = ' ') {
            if (_position + width >= _length)
                return *this;
            uint8_t intWidth = decimalLength(value);
            if (intWidth > width)
                return *this;
            pad(width - intWidth, padding);
            core::ultoa((int16_t)value, ptr(), 10);
            seek(intWidth);
            return *this;
        }
        StringBuilder& append(uint16_t value) {
            uint8_t intWidth = decimalLength(value);
            if (_position + intWidth >= _length)
                return *this;

            core::ultoa(value, ptr(), 10);
            seek(strlen(ptr()));
            return *this;
        }
        StringBuilder& appendHex(uint16_t value) {
            size_t width = 4;
            if (_position + width >= _length)
                return *this;

            core::ultoa(value, ptr(), 16);
            rightAlign(ptr(), width);
            seek(strlen(ptr()));
            return *this;
        }

        StringBuilder& append(uint8_t value, int8_t width, char padding = ' ') {
            if (_position + width >= _length)
                return *this;
            uint8_t intWidth = decimalLength(value);
            if (intWidth > width)
                return *this;
            pad(width - intWidth, padding);
            core::itoa((uint16_t)value, ptr(), 10);
            seek(intWidth);
            return *this;
        }
        StringBuilder& append(uint8_t value) {
            uint8_t intWidth = decimalLength(value);
            if (_position + intWidth >= _length)
                return *this;

            core::ultoa((uint16_t)value, ptr(), 10);
            seek(strlen(ptr()));
        }
        StringBuilder& appendHex(uint8_t value) {
            size_t width = 2;
            if (_position + width >= _length)
                return *this;

            core::ultoa((uint16_t)value, ptr(), 16);
            rightAlign(ptr(), width);
            seek(strlen(ptr()));
            return *this;
        }

        uint8_t decimalLength(uint8_t value) const {
            if (value < 10)
                return 1;
            if (value < 100)
                return 2;
            return 3;
        }

        uint8_t decimalLength(uint16_t value) const {
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

        uint8_t decimalLength(uint32_t value) const {
            if (value <= UINT16_MAX)
                return decimalLength((uint16_t) value);
            uint8_t length = 4;
            for (uint32_t i = 10000; i <= value; i *= 10) {
                length++;
            }
            return length;
        }

        uint8_t decimalLength(int8_t value) const {
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

        uint8_t decimalLength(int16_t value) const {
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

        uint8_t decimalLength(int32_t value) const {
            if (value <= INT16_MAX && value >= INT16_MIN)
                return decimalLength((int16_t) value);
            if (value < 0) {
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

        void lineBreak() {
            append(::core::cstrings::newLine(cstrings::LF));
        }

        void lineBreak(::core::cstrings::NewLineMode mode) {
            append(::core::cstrings::newLine(mode));
        }

        void clear() {
			_position = 0;
			_buffer[_position] = '\0';
        }

        const char* toCString() const { return _buffer;}

        core::CString toSharedCString() {
            return core::cstrings::toSharedCString(toCString());
        }

    private:
        inline char* ptr() const { return &_buffer[_position]; }

        inline void seek(size_t width) {
            _position += width;
        }

        inline void pad(size_t width, char padding) {
            memset(ptr(), padding, width);
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
        size_t _length;
        size_t _position;
    };
}

#endif //FIRMWARE_STRINGBUILDER_H
