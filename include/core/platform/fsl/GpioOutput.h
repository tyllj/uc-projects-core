#ifndef HAL_GPIOOUTPUT_H_
#define HAL_GPIOOUTPUT_H_

#include <stdint.h>
#include <fsl_gpio.h>
#include "core/hw/Pin.h"


namespace fslCore {
    class GpioInput : public core::hw::Input {
    public:

        GpioInput() = default; // TODO: Find out if this default constructor can be removed.

        GpioInput(GPIO_Type *gpio, uint32_t pin) :
                _gpio(gpio),
                _pin(pin) {
            gpio_pin_config_t config = {.pinDirection = kGPIO_DigitalInput, .outputLogic = 0};
            GPIO_PinInit(gpio, pin, &config);
        }

        volatile bool isHigh() override {
            return GPIO_PinRead(_gpio, _pin) == 1;
        }

        volatile bool isLow() override {
            return GPIO_PinRead(_gpio, _pin) == 0;
        }


    protected:
        GpioInput(GPIO_Type *gpio, uint32_t pin, gpio_pin_direction_t direction) : _gpio(gpio), _pin(pin) {
            gpio_pin_config_t config = {.pinDirection = direction, .outputLogic = 0};
            GPIO_PinInit(gpio, pin, &config);
        }

        GPIO_Type *_gpio;
        uint32_t _pin;
    };

    class GpioOutput : public GpioInput, public core::hw::Output {
    public:
        GpioOutput() = default; // TODO: Find out if this default constructor can be removed.

        GpioOutput(GPIO_Type *gpio, uint32_t pin) : GpioInput(gpio, pin, kGPIO_DigitalOutput) {}

        void set(bool value) final {
            GPIO_PinWrite(_gpio, _pin, (uint8_t) value);
        }

        volatile bool isHigh() final {
            return this->GpioInput::isHigh();
        }

        volatile bool isLow() final {
            return this->GpioInput::isLow();
        }
    };
}
#endif
