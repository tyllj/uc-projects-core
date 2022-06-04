//
// Created by tyll on 09.02.22.
//

#include <stdint.h>

#ifndef UC_CORE_BITS_H
#define UC_CORE_BITS_H

// Function definitions are guarded to avoid clashes with their Arduino counterparts if the Arduino lib is used.

#ifndef makeWord
inline uint16_t makeWord(uint8_t h, uint8_t l) {
    return uint16_t (((uint16_t) h << 8) | (uint16_t) l);
}
#endif
#ifndef lowByte
inline uint8_t lowByte(uint16_t word) {
    return uint8_t((word) & 0xff) ;
}
#endif
#ifndef highByte
inline uint8_t highByte(uint16_t word) {
    return uint8_t((word) >> 8);
}
#endif
#ifndef bitRead
#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#endif
#ifndef bitSet
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#endif
#ifndef bitClear
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#endif
#ifndef bitToggle
#define bitToggle(value, bit) ((value) ^= (1UL << (bit)))
#endif
#ifndef bitWrite
#define bitWrite(value, bit, bitvalue) ((bitvalue) ? bitSet(value, bit) : bitClear(value, bit))
#endif
#ifndef bit
#define bit(b) (1UL << (b))
#endif

#endif //UC_CORE_BITS_H
