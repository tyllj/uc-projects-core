//
// Created by tu.jungke on 10.11.2022.
//

#ifndef __WIN32FT4222SPIDEVICE_H__
#define __WIN32FT4222SPIDEVICE_H__

#include "Win32Ft4222.h"
#include "core/hw/spi/SpiDevice.h"

namespace core { namespace platform { namespace pc {
    class Win32Ft4222SpiDevice : public core::hw::spi::SpiDevice {
    public:

        Win32Ft4222SpiDevice(Win32Ft4222SpiDevice&) = delete;
        Win32Ft4222SpiDevice(Win32Ft4222SpiDevice&&) = delete;


        Win32Ft4222SpiDevice(Win32Ft4222 ft4222SpiDevice, uint32_t deviceLocation) {

        }

        ~Win32Ft4222SpiDevice() {

        };

        auto read(uint8_t *buffer, size_t n) -> size_t final {
            return 0;
        }

        auto write(const uint8_t *buffer, size_t n) -> void final {

        }

        size_t transferFullDuplex(const uint8_t *sendBuffer, uint8_t *receiveBuffer, size_t n) final {

        }

    };
}}}

#endif //__WIN32FT4222SPIDEVICE_H__
