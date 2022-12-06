//
// Created by tu.jungke on 10.11.2022.
//

#ifndef __WIN32FT4222SPIDEVICESLAVE_H__
#define __WIN32FT4222SPIDEVICESLAVE_H__

#include <mutex>
#include "Win32Ft4222.h"
#include "core/hw/spi/SpiDevice.h"

namespace core { namespace platform { namespace pc {
    class Win32Ft4222SpiDeviceSlave : public core::hw::spi::SpiDevice {
    public:
        Win32Ft4222SpiDeviceSlave(Win32Ft4222SpiDeviceSlave &) = delete;

        Win32Ft4222SpiDeviceSlave(Win32Ft4222SpiDeviceSlave &&) = delete;

        Win32Ft4222SpiDeviceSlave(const char* deviceName, std::mutex& mtx, FT4222_ClockRate clockRate = FT4222_ClockRate::SYS_CLK_80) : _port(nullptr), _mtx(mtx) {
            if (_ftLib.openEx(const_cast<char *>(deviceName), FT_OPEN_BY_DESCRIPTION, &_port) != FT_OK)
                throw std::runtime_error("Could not open FT4222 SPI device.");
            if (_ftLib.setClock(_port, clockRate) != FT4222_OK)
                throw std::runtime_error("Could not set system clock on FT4222 SPI device.");
            if (_ftLib.spiSlaveInitEx(_port, SPI_SlaveProtocol::SPI_SLAVE_NO_PROTOCOL) != FT4222_OK)
                throw std::runtime_error("Could not initialize FT4222 SPI device as SPI slave.");
        }

        ~Win32Ft4222SpiDeviceSlave() override {
            if (_port)
                _ftLib.close(_port);
        };

        auto read(uint8_t *buffer, size_t n) -> size_t final {
            auto _ = std::lock_guard(_mtx);
            auto sizeTransferred = uint16_t();
            _ftLib.spiSlaveRead(_port, buffer, n, &sizeTransferred);
            return sizeTransferred;
        }

        auto write(const uint8_t *buffer, size_t n) -> void final {
            auto _ = std::lock_guard(_mtx);
            auto sizeTransferred = uint16_t();
            while (n > 0) {
                _ftLib.spiSlaveWrite(_port, const_cast<uint8 *>(buffer + sizeTransferred), n, &sizeTransferred);
                n -= sizeTransferred;
            }
        }

        auto transferFullDuplex(const uint8_t *sendBuffer, uint8_t *receiveBuffer, size_t n) -> size_t final {
            throw std::logic_error("Not implemented.");
        }

        auto reset() -> void {
            _ftLib.spiReset(_port);
        }

    private:
        Win32Ft4222 _ftLib;
        FT_HANDLE _port = nullptr;
        std::mutex& _mtx;
    };
}}}
#endif //__WIN32FT4222SPIDEVICESLAVE_H__
