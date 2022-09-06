//
// Created by tyll on 2022-03-24.
//

#ifndef UC_CORE_PIN_H
#define UC_CORE_PIN_H

namespace core { namespace hw {
    class Input {
        virtual bool isHigh() = 0;
        virtual bool isLow() {
            return !isHigh();
        }
    };

    class Output : public Input{
    public:
        virtual void set(bool value) = 0;
        void setHigh() {
            set(true);
        }
        void setLow() {
            set(false);
        }
    };
}}

#endif //SGLOGGER_PIN_H