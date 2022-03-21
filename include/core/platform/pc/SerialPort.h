//
// Created by tyll on 2022-01-22.
//

#ifndef SGLOGGER_PC_SERIALPORT_H
#define SGLOGGER_PC_SERIALPORT_H

#include <stdint.h>
#include "core/io/Stream.h"
#include "RS-232/rs232.h"

namespace core { namespace io { namespace ports {
    class SerialPort : public Stream {
    public:
        ~SerialPort() {
            SerialPort::close();
        }

        void setPortName(const char* portName) {
            if (!_isOpen)
                _portNumber = RS232_GetPortnr(portName);
        };

        void setBaudRate(int32_t baudRate) {
            if (!_isOpen)
                _baudRate = baudRate;
        }

        bool canRead() const override { return true; }
        bool canWrite() const override { return true; }
        int32_t getLength() const override { return 0; }
        int32_t getPosition() const override { return 0; }

        void open() {
            if (!_isOpen && RS232_OpenComport(_portNumber, _baudRate,"8N1", 0) == 0)
                _isOpen = true;
        }

        void close() override {
            if (_isOpen)
                RS232_CloseComport(_portNumber);
        }

        void flush() override {  }

        int32_t readByte() override {
            uint8_t byte;
            if (!_isOpen || RS232_PollComport(_portNumber, &byte, 1) != 1)
                return -1;
            return (int32_t) byte;
        }

        void writeByte(uint8_t byte) override {
            if (_isOpen)
                RS232_SendByte(_portNumber, byte);
        }

    private:
        bool _isOpen = false;
        int _portNumber =  4;
        int _baudRate = 9600;
    };
}}}
#endif //SGLOGGER_SERIALPORT_H
