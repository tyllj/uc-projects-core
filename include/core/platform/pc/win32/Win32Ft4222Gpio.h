//
// Created by tu.jungke on 10.11.2022.
//

#ifndef __WIN32FT4222GPIO_H__
#define __WIN32FT4222GPIO_H__

#include <mutex>
#include <stdint.h>
#include "core/hw/Pin.h"
#include "ftdiimports/LibFT4222.h"
#include "Win32Ft4222.h"

namespace core { namespace platform { namespace pc {
    class Win32Ft4222GpioPort {
    public:
        class GpioPin : public core::hw::Output {
        public:
            GpioPin(GpioPin&) = delete;
            GpioPin(GpioPin&&) = delete;

            auto set(bool level) -> void final {
                _port.set(_pin, static_cast<core::hw::PinLevel>(level));
            }

            auto isHigh() -> bool final {
                return _port.isHigh(_pin);
            }
        private:
            GpioPin(Win32Ft4222GpioPort& port, uint8_t pin) : _port(port), _pin(pin) {}
            Win32Ft4222GpioPort& _port;
            uint8_t _pin;
            friend class Win32Ft4222GpioPort;
        };

        Win32Ft4222GpioPort(Win32Ft4222GpioPort&) = delete;
        Win32Ft4222GpioPort(Win32Ft4222GpioPort&&) = delete;

        Win32Ft4222GpioPort(const char* deviceName, const GPIO_Dir gpioDir[4], std::mutex& mtx, FT4222_ClockRate clockRate = FT4222_ClockRate::SYS_CLK_80) :
            _port(nullptr),
            _mtx(mtx),
            _pin0(*this, 0),
            _pin1(*this, 1),
            _pin2(*this, 2),
            _pin3(*this, 3) {
            if (_ftLib.openEx(const_cast<char *>(deviceName), FT_OPEN_BY_DESCRIPTION, &_port) != FT_OK)
                throw std::runtime_error("Could not open FT4222 SPI device.");
            if (_ftLib.setClock(_port, clockRate) != FT4222_OK)
                throw std::runtime_error("Could not set system clock on FT4222 SPI device.");
            if (_ftLib.gpioInit(_port, const_cast<GPIO_Dir*>(gpioDir)) != FT4222_OK)
                throw std::runtime_error("Could not initialize FT4222 GPIO port.");
        }

        auto isHigh(uint8_t pinNo) -> bool {
            auto _ = std::lock_guard(_mtx);
            auto result = WINBOOL();
            if (_ftLib.gpioRead(_port, static_cast<GPIO_Port>(pinNo), &result) != FT4222_OK)
                throw std::runtime_error("Could not read pin.");
            return result;
        }

        auto isLow(uint8_t pinNo) -> bool { return !isHigh(pinNo); }

        auto set(uint8_t pinNo, core::hw::PinLevel level) -> void {
            auto _ = std::lock_guard(_mtx);
            if (_ftLib.gpioWrite(_port, static_cast<GPIO_Port>(pinNo), static_cast<WINBOOL>(level)) != FT4222_OK)
                throw std::runtime_error("Could not set pin.");
        }

        auto setLow(uint8_t pinNo) -> void { return set(pinNo, core::hw::PinLevel::Low); }
        auto setHigh(uint8_t pinNo) -> void { return set(pinNo, core::hw::PinLevel::High); }

        auto& pin0() { return _pin0; }
        auto& pin1() { return _pin1; }
        auto& pin2() { return _pin2; }
        auto& pin3() { return _pin3; }

    private:
        Win32Ft4222 _ftLib;
        FT_HANDLE _port = nullptr;
        std::mutex& _mtx;
        GpioPin _pin0;
        GpioPin _pin1;
        GpioPin _pin2;
        GpioPin _pin3;
    };
}}}
#endif //__WIN32FT4222GPIO_H__
