//
// Created by tyll on 2022-01-26.
//

#ifndef SGLOGGER_STRINGMAP_H
#define SGLOGGER_STRINGMAP_H
#include <stdint.h>
#include <string.h>
#include "core/CStrings.h"

namespace core { namespace cli {
    template<typename TValue, unsigned buckets>
    class StringMap {
    public:
        void addOrUpdate(const char* key, TValue value) {
            _StringMapEntry* entry;
            if (cstrings::isNullOrEmpty(key))
                return;

            _StringMapEntry** currentRef = &_buckets[bucketOf(key)];
            while (*currentRef != nullptr && !cstrings::equals((*currentRef)->_key, key))
                currentRef = &((*currentRef)->_next);

            entry = new _StringMapEntry { ._key = key, ._value = value, ._next =  (*currentRef) ? (*currentRef)->_next : nullptr};
            (*currentRef) = entry;
        }

        TValue& getOrDefault(const char* key, TValue& fallbackValue) const {
            if (cstrings::isNullOrEmpty(key))
                return fallbackValue;

            _StringMapEntry* current = _buckets[bucketOf(key)];
            while (current != nullptr) {
                if (cstrings::equals(current->_key, key))
                    return current->_value;
                current = current->_next;
            }

            return fallbackValue;
        }
        bool contains(const char* key) const {
            if (cstrings::isNullOrEmpty(key))
                return false;

            _StringMapEntry* current = _buckets[bucketOf(key)];
            while (current != nullptr) {
                if (cstrings::equals(current->_key, key))
                    return true;
                current = current->_next;
            }

            return false;
        }

        ~StringMap() {
            for (unsigned i = 0; i < buckets; i++) {
                _StringMapEntry* current = _buckets[i];
                _StringMapEntry* next = current;
                while (current != nullptr) {
                    next = current->_next;
                    delete current;
                    current = next;
                }
            }
        }

    private:
        struct _StringMapEntry {
            const char* _key;
            TValue _value;
            _StringMapEntry* _next;
        };

        _StringMapEntry* _buckets[buckets];

        unsigned bucketOf(const char* key) const {
            return ((unsigned) key[0] + (unsigned) key[1]) % buckets;
        }

    };
}}
#endif //SGLOGGER_STRINGMAP_H
