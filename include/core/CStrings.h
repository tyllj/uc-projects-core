//
// Created by tyll on 2022-01-26.
//

#ifndef UC_CORE_CSTRINGS_H
#define UC_CORE_CSTRINGS_H

#include <stdint.h>
#include <string.h>
#include <cctype>
#include "shared_ptr.h"
#include "core/Math.h"
#include "core/Error.h"

#ifdef _MSC_VER
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#endif

namespace core {
    class CString : public core::shared_ptr<char[]> {
    public:
        constexpr CString() : core::shared_ptr<char[]>() {}
        CString(const CString& other) : core::shared_ptr<char[]>(other) {}
        CString(CString&& other) : core::shared_ptr<char[]>(other) {}
        CString(const core::shared_ptr<char[]>& other) : core::shared_ptr<char[]>(other) {}
        CString(core::shared_ptr<char[]>&& other) : core::shared_ptr<char[]>(other) {}
        explicit CString(size_t size) : core::shared_ptr<char[]>(size) {}
        explicit CString(int32_t size) : core::shared_ptr<char[]>(static_cast<size_t>(size)) {}
        CString(const char * str) : core::shared_ptr<char[]>(strlen(str) + 1) {
            strcpy(get(), str);
        }
        CString(char * str) : core::shared_ptr<char[]>(strlen(str) + 1) {
            strcpy(get(), str);
        }

        template<typename TNativeStringType>
        CString(const TNativeStringType& str) : core::shared_ptr<char[]>(str.TNativeStringType::length() + 1) {
            strcpy(get(), str.TNativeStringType::c_str());
        }

        CString& operator=(const CString& other) {
            core::shared_ptr<char[]>::operator=(other);
            return *this;
        }

        operator char*() const { return get(); }
        operator const char*() const { return get(); }
    };

    template<typename TTo>
    inline TTo cstring_cast(const CString& str) {
        return TTo(str.get());
    }

    template<>
    inline CString cstring_cast<CString>(const CString& str) {
        return str;
    }

    template<>
    inline const char* cstring_cast<const char*>(const CString& str) {
        return str.get();
    }
}

namespace core { namespace cstrings {

    enum class Error {
        SubstringNotFound
    };

    template<size_t n>
    inline void copyTo(char (&destination)[n], const char* source) {
        strncpy(reinterpret_cast<char *>(&destination), source, n - 1);
    }

    template<size_t bufferSize>
    inline void copyTo(char (&destination)[bufferSize], const char* source, size_t n) {
        strncpy(reinterpret_cast<char *>(&destination), source, min(bufferSize - 1, n));
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

    template<size_t n>
    inline uint8_t split(char* str, const char spliterator, char* (&dest)[n]) {
        return split(str, spliterator, dest, n);
    }

    inline void removeWhitespaces(char* destination, const char* source) {
        while (*source != '\0') {
            if(!isspace(*source)) {
                *destination = *source;
                destination++;
            }
            source++;
        }
        *destination = '\0';
    }

    inline void removeWhitespaces(char* inplace) {
        removeWhitespaces(inplace, inplace);
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

    inline ErrorOr<size_t> indexOf(const char* haystack, const char* needle) {
        const char* ptr = strstr(haystack, needle);
        if (ptr == nullptr)
            return Error::SubstringNotFound;
        else
            return static_cast<int32_t>(ptr - haystack);
    }

    inline ErrorOr<size_t> indexOf(const char* haystack, const char needle) {
        const char* ptr = strchr(haystack, needle);
        if (ptr == nullptr)
            return Error::SubstringNotFound;
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

    inline void empty(char* str) {
        str[0] = '\0';
    }

    inline const char* empty() {
        return "";
    }

    template<typename TFunctor>
    inline bool any(const char* str, TFunctor predicate) {
        while (*str) {
            if (predicate(*str))
                return true;
            str++;
        }
        return false;
    }

    template<typename TFunctor>
    inline bool all(const char* str, TFunctor predicate) {
        while (*str) {
            if (!predicate(*str))
                return false;
            str++;
        }
        return true;
    }


    inline bool contains(const char* haystack, char needle) {
        return any(haystack, [=](char c) { return c == needle; });
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
