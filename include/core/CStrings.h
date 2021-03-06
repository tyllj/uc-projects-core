//
// Created by tyll on 2022-01-26.
//

#ifndef UC_CORE_CSTRINGS_H
#define UC_CORE_CSTRINGS_H

#include <stdint.h>
#include <string.h>
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

    inline const char* empty() {
        return "";
    }

    enum NewLineMode {
        POSIX,
        MSDOS
    };

    constexpr const char * newLine(NewLineMode mode) {
        switch (mode) {
            case POSIX: return "\n";
            case MSDOS: return "\r\n";
            default: return cstrings::empty();
        }
    }
}}
#endif //UC_CORE_CSTRINGS_H
