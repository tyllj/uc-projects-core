//
// Created by tyll on 2022-01-22.
//

#ifndef UC_CORE_PC_SERIALPORT_H
#define UC_CORE_PC_SERIALPORT_H

#include <stdint.h>
#include "core/io/Stream.h"
#include "RS-232/rs232.h"

namespace core { namespace io { namespace ports {
    class SerialPort : public Stream {
    public:
        explicit SerialPort(const char* portName) : _isOpen(false), _portNumber(RS232_GetPortnr(portName)), _baudRate(9600) {

        }

        ~SerialPort() {
            SerialPort::close();
        }

        void setBaudRate(int32_t baudRate) {
            if (!_isOpen)
                _baudRate = baudRate;
        }

        bool canRead() const final { return true; }
        bool canWrite() const final { return true; }
        size_t getLength() const final { return 0; }
        size_t getPosition() const final { return 0; }

        void open() {
            if (!_isOpen && RS232_OpenComport(_portNumber, _baudRate,"8N1", 0) == 0)
                _isOpen = true;
        }

        void close() final {
            if (_isOpen)
                RS232_CloseComport(_portNumber);
        }

        void flush() final {  }

        int32_t readByte() final {
            uint8_t byte;
            if (!_isOpen || RS232_PollComport(_portNumber, &byte, 1) != 1)
                return -1;
            return (int32_t) byte;
        }

        size_t read(uint8_t* buffer, size_t offset, size_t count) final {
            return RS232_PollComport(_portNumber, &buffer[offset], count);
        }

        void writeByte(uint8_t byte) final {
            if (_isOpen)
                RS232_SendByte(_portNumber, byte);
        }

        void write(const uint8_t* buffer, size_t offset, size_t count) final {
            if (_isOpen)
                RS232_SendBuf(_portNumber, const_cast<unsigned char*>(&buffer[offset]), count);
        }

    private:
        bool _isOpen;
        int _portNumber;
        int _baudRate;
    };
}}}
#endif //UC_CORE_SERIALPORT_H
