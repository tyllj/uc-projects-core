//
// Created by tu.jungke on 10.11.2022.
//

#ifndef __SPIDEVICE_H__
#define __SPIDEVICE_H__

#include <stdint.h>

namespace core { namespace hw { namespace spi {
    class SpiDevice {
    public:
        virtual ~SpiDevice() = default;
        virtual auto read(uint8_t* buffer, size_t n) -> size_t = 0;
        template<size_t n>
        auto read(uint8_t (&buffer)[n]) -> uint8_t { return read(buffer, n); }

        virtual auto readByte() -> int32_t {
            uint8_t buf;
            if (read(&buf, 1) == 1)
                return buf;
            return -1;
        }

        virtual auto write(const uint8_t* buffer, size_t n) -> void = 0;
        template<size_t n>
        auto write(uint8_t (&buffer)[n]) -> void { write(buffer, n); }

        virtual auto writeByte(uint8_t byte) -> void {
            write(&byte, 1);
        }

        virtual auto transferFullDuplex(const uint8_t* sendBuffer, uint8_t* receiveBuffer, size_t n) -> size_t = 0;
    };
}}}
#endif //__SPIDEVICE_H__
