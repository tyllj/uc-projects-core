//
// Created by tyll on 2022-01-27.
//

#ifndef SGLOGGER_TEMPLATEDMAP_H
#define SGLOGGER_TEMPLATEDMAP_H

#include <stdint.h>

namespace core {namespace collections {namespace TemplatedMap {
    template <
            typename TKey,
            typename TValue,
            typename TTail,
            TKey key,
            TValue value
    >
    class head {
    public:
        const bool isEmpty() const { return false; }

        const TValue get(TKey k) const {
            return k == key
                   ? value
                   : TTail().get(k);
        }

        const bool contains(TKey k) const {
            return (k == key || TTail().contains(k));
        }

        const uint16_t count() const { return 1 + TTail().count(); }

        template<TKey newKey, TValue newValue>
        const auto add() const {
            if (contains(newKey))
                return head<TKey, TValue, TTail, key, value>();
            return head<TKey, TValue, head<TKey, TValue, TTail, key, value>, newKey, newValue>();
        }
    };

    template <typename TKey, typename TValue, TValue def>
    class nothing {
    public:
        const bool isEmpty() const { return true; }

        const TValue get(TKey k) const { return def; }

        const bool contains(TKey k) const { return false; }

        const uint16_t count() const { return 0; }

        template<TKey newKey, TValue newValue>
        const auto add() const {
            head<TKey, TValue, nothing<TKey, TValue, def>, newKey, newValue> newHead;
            return newHead;
        }
    };

    template <typename TKey, typename TValue, TValue def>
    constexpr nothing<TKey, TValue, def> empty() {
        const nothing<TKey, TValue, def> n;
        return n;
    }
}}}
#endif //SGLOGGER_TEMPLATEDMAP_H
