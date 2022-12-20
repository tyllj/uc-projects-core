//
// Created by tyll on 2022-03-24.
//

#ifndef UC_CORE_PIN_H
#define UC_CORE_PIN_H

namespace core { namespace hw {
    enum class PinLevel : uint8_t {
        Low = 0,
        High = 1
    };

    class Input {
        virtual bool isHigh() = 0;
        virtual bool isLow() {
            return !isHigh();
        }
    };

    class Output : public Input{
    public:
        virtual void set(PinLevel value) = 0;
        void setHigh() {
            set(PinLevel::High);
        }
        void setLow() {
            set(PinLevel::Low);
        }
    };
}}

#endif //UC_CORE_PIN_H