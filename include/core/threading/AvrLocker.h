//
// Created by tyll on 10.02.22.
//

#ifndef FIRMWARE_AVRLOCKER_H
#define FIRMWARE_AVRLOCKER_H

#ifdef __AVR__

#include "core/Bits.h"
#include "avr/interrupt.h"

namespace core { namespace threading {
        class AvrLocker {
        public:
            static void lock() {
                _iFlagBeforLock = bitRead(SREG, 7);
                if (_iFlagBeforLock)
                    cli();
            }

            static void unlock() {
                if (_iFlagBeforLock)
                    sei();
            }

            static inline bool _iFlagBeforLock = false;
        };
}}
#endif //__AVR__
#endif //FIRMWARE_AVRLOCKER_H
