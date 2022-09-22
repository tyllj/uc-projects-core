//
// Created by tyll on 2022-01-27.
//

#ifndef UC_CORE_TEMPLATEDMAP_H
#define UC_CORE_TEMPLATEDMAP_H

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
        bool isEmpty() const { return false; }

        const TValue get(TKey k) const {
            return k == key
                   ? value
                   : TTail().get(k);
        }

        bool contains(TKey k) const {
            return (k == key || TTail().contains(k));
        }

        uint16_t count() const { return 1 + TTail().count(); }

        template<TKey newKey, TValue newValue>
        auto add() const {
            if (contains(newKey))
                return head<TKey, TValue, TTail, key, value>();
            return head<TKey, TValue, head<TKey, TValue, TTail, key, value>, newKey, newValue>();
        }
    };

    template <typename TKey, typename TValue, TValue def>
    class nothing {
    public:
        bool isEmpty() const { return true; }

        const TValue get(TKey k) const { return def; }

        bool contains(TKey k) const { return false; }

        uint16_t count() const { return 0; }

        template<TKey newKey, TValue newValue>
        auto add() const {
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
#endif //UC_CORE_TEMPLATEDMAP_H
