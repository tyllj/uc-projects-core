//
// Created by tyll on 2022-01-27.
//

#ifndef SGLOGGER_TEMPLATEDSTRINGMAP_H
#define SGLOGGER_TEMPLATEDSTRINGMAP_H

#include <stdint.h>
#include "core/CStrings.h"

namespace core {namespace collections {namespace TemplatedStringMap {

    template<unsigned buckets>
    constexpr unsigned bucketOf(const char* key) {
        return ((unsigned) key[0] + (unsigned) key[1]) % buckets;
    }

    template <
            typename TValue,
            typename TTail,
            const char* key,
            TValue value
    >
    class head {
    public:
        const bool isEmpty() const { return false; }

        const TValue get(const char* k) const {
            return core::cstrings::equals(k, key)
                   ? value
                   : TTail().get(k);
        }

        const bool contains(const char* k) const {
            return (core::cstrings::equals(k, key) || TTail().contains(k));
        }

        const uint16_t count() const { return 1 + TTail().count(); }

        template<const char* newKey, TValue newValue>
        const auto add() const {
            if (contains(newKey))
                return head<TValue, TTail, key, value>();
            return head<TValue, head<TValue, TTail, key, value>, newKey, newValue>();
        }
    };

    template <typename TValue, TValue def>
    class nothing {
    public:
        const bool isEmpty() const { return true; }

        const TValue get(const char* k) const { return def; }

        const bool contains(const char* k) const { return false; }

        const uint16_t count() const { return 0; }

        template<const char* newKey, TValue newValue>
        const auto add() const {
            head<TValue, nothing< TValue, def>, newKey, newValue> newHead;
            return newHead;
        }
    };

    template <typename TValue, TValue def>
    constexpr nothing<TValue, def> empty() {
        const nothing<TValue, def> n;
        return n;
    }
}}}
#endif //SGLOGGER_TEMPLATEDSTRINGMAP_H