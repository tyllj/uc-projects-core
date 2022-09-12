//
// Created by tyll on 2022-01-26.
//

#ifndef UC_CORE_CSTRINGS_H
#define UC_CORE_CSTRINGS_H

#include <stdint.h>
#include <string.h>
#include "shared_ptr.h"

#define SUBSTRING_NOT_FOUND -1

namespace core {
    typedef core::shared_ptr<char[]> CString;
}

namespace core { namespace cstrings {

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

    inline bool isNullOrEmpty(const char* str) {
        return (str == nullptr || str[0] == '\0');
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

    inline int32_t indexOf(const char* haystack, const char* needle) {
        const char* ptr = strstr(haystack, needle);
        if (ptr == NULL)
            return -1;
        else
            return static_cast<int32_t>(ptr - haystack);
    }

    inline int32_t indexOf(const char* haystack, const char needle) {
        const char* ptr = strchr(haystack, needle);
        if (ptr == NULL)
            return -1;
        else
            return static_cast<int32_t>(ptr - haystack);
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
