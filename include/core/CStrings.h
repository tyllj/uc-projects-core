//
// Created by tyll on 2022-01-26.
//

#ifndef UC_CORE_CSTRINGS_H
#define UC_CORE_CSTRINGS_H

#include <stdint.h>
#include <string.h>
#include <cctype>
#include "shared_ptr.h"

#define SUBSTRING_NOT_FOUND -1

namespace core {
    typedef core::shared_ptr<char[]> CString;
}

namespace core { namespace cstrings {

    template<size_t n>
    inline void copy(char (&destination)[n], const char* source) {
        strncpy(reinterpret_cast<char *>(&destination), source, n);
    }

    inline uint8_t split(char* str, const char spliterator, char** dest, uint8_t limit) {
        if (limit == 0)
            return 0;

        uint8_t c = 0;
        uint8_t i = 0;

        dest[0] = str;
        c++;

        while (c <= limit && str[i] != '\0')  {
            if (str[i] == spliterator) {
                while (str[i] == spliterator) {
                    str[i] = '\0';
                    i++;
                }
                dest[c] = &str[i];
                c++;
            }
            i++;
        }
        return c;
    }

    inline void trim(char* destination, const char* source) {
        while (*source != '\0') {
            if(!isspace(*source)) {
                *destination = *source;
                destination++;
            }
            source++;
        }
        *destination = '\0';
    }

    inline void trim(char* inplace) {
        trim(inplace, inplace);
    }

    inline void toUpper(char * destination, const char* source) {
        while (*source != '\0') {
            *destination = static_cast<char>(toupper( *source));
            destination++;
            source++;
        }
    }

    inline void toUpper(char* inplace) {
        toUpper(inplace, inplace);
    }

    inline void toLower(char * destination, const char* source) {
        while (*source != '\0') {
            *destination = static_cast<char>(tolower( *source));
            destination++;
            source++;
        }
    }

    inline void toLower(char* inplace) {
        toLower(inplace, inplace);
    }

    inline bool isNullOrEmpty(const char* str) {
        return (str == nullptr || str[0] == '\0');
    }

    inline bool isNullOrWhitespace(const char* str) {
        if (isNullOrEmpty(str))
            return true;
        while (*str) {
            if (!isblank(*str))
                return false;
        }
        return true;
    }

    inline bool equals(const char* str0, const char* str1) {
        return strcmp(str0, str1) == 0;
    }

    inline bool equalsCaseInvariant(const char* str0, const char* str1) {
        return strcasecmp(str0, str1) == 0;
    }

    inline size_t length(const char* str) {
        return strlen(str);
    }

    inline bool startsWith(const char * haystack, const char * needle)
    {
        while(*needle) {
            if(*needle++ != *haystack++)
                return false;
        }
        return true;
    }


    inline int32_t indexOf(const char* haystack, const char* needle) {
        const char* ptr = strstr(haystack, needle);
        if (ptr == NULL)
            return SUBSTRING_NOT_FOUND;
        else
            return static_cast<int32_t>(ptr - haystack);
    }

    inline int32_t indexOf(const char* haystack, const char needle) {
        const char* ptr = strchr(haystack, needle);
        if (ptr == NULL)
            return SUBSTRING_NOT_FOUND;
        else
            return static_cast<int32_t>(ptr - haystack);
    }

    inline void subString(char* destination, const char* source, size_t startIndex) {
        if (startIndex >= length(source))
            return;
        source += startIndex;
        while (*source != '\0') {
            *destination = *source;
            destination++;
            source++;
        }
    }

    inline void subString(char* inplace, size_t startIndex) {
        subString(inplace, inplace, startIndex);
    }

    inline void subString(char* destination, const char* source, size_t startIndex, size_t length) {
        if (length == 0 || startIndex >= cstrings::length(source))
            return;
        source += startIndex;
        while (*source != '\0' && length-- > 0) {
            *destination = *source;
            destination++;
            source++;
        }
    }

    inline void subString(char* inplace, size_t startIndex, size_t length) {
        subString(inplace, inplace, startIndex, length);
    }

    inline core::CString toSharedCString(const char* str) {
        CString result = CString(length(str) + 1);
        strcpy(result.get(), str);
        return result;
    }

    inline const char* empty() {
        return "";
    }

    enum NewLineMode {
        LF,
        CRLF,
        CR
    };

    constexpr const char * newLine(NewLineMode mode) {
        switch (mode) {
            case LF: return "\n";
            case CRLF: return "\r\n";
            case CR: return "\r";
            default: return cstrings::empty();
        }
    }
}}
#endif //UC_CORE_CSTRINGS_H
