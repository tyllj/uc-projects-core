//
// Created by tyll on 2022-03-24.
//

#ifndef UC_CORE_SHIFTREGISTERPARALLELOUT_H
#define UC_CORE_SHIFTREGISTERPARALLELOUT_H

#ifndef LSBFIRST
#define LSBFIRST 0
#endif
#ifndef MSBFIRST
#define MSBFIRST 1
#endif

#include <stdint.h>

namespace core { namespace hw {
    template<typename TOutputImpl>
    class ShiftRegisterParallelOut {
    public:
        explicit ShiftRegisterParallelOut(TOutputImpl dataPin , TOutputImpl clockPin, TOutputImpl latchPin, uint8_t bitOrder);
        void shiftOut(uint8_t value) {
            uint8_t i;

            for (i = 0; i < 8; i++)  {
                if (_bitOrder == LSBFIRST)
                    _dataPin.set(!!(value & (1 << i)));
                else
                    _dataPin.set(!!(value & (1 << (7 - i))));

                _clockPin.setHigh();
                _clockPin.setLow();
            }
        }
        void latch() {
            _latchPin.setLow();
        }
        void unlatch() {
            _latchPin.setHigh();
        }
    private:
        const uint8_t _bitOrder;
        TOutputImpl _dataPin;
        TOutputImpl _clockPin;
        TOutputImpl _latchPin;
    };
}}
#endif //UC_CORE_SHIFTREGISTERPARALLELOUT_H
