//
// Created by tyll on 29.09.22.
//

#ifndef UC_CORE_WIN32SERIALPORT_H
#define UC_CORE_WIN32SERIALPORT_H

#if defined(_WIN32)

#include <stdio.h>
#include <windows.h>
#include <string.h>
#include <stdexcept>
#include "core/io/Stream.h"
#include "core/StringBuilder.h"

namespace core { namespace platform { namespace pc {
    class Win32SerialPort : public io::Stream {
    public:
        explicit Win32SerialPort(const char* portName) : _baudRate(9600), _isOpen(false) {
            strncpy(_portName, portName, sizeof(_portName));
        }

        ~Win32SerialPort() {
            Win32SerialPort::close();
        }

        void baudRate(int32_t baudRate) {
            if (!_isOpen)
                _baudRate = baudRate;
        }

        bool canRead() const final { return true; }
        bool canWrite() const final { return true; }
        size_t length() const final { return 0; }
        size_t position() const final { return 0; }

        void open() {
            if (_isOpen)
                return;

            char devicePath[16] = {0};
            snprintf(devicePath, sizeof(devicePath), "\\\\.\\%s", _portName);
            _handle = CreateFileA(devicePath, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
            if(_handle == INVALID_HANDLE_VALUE) {
                throw std::runtime_error("Could not create handle.");
            }

            char modeString[128] = {0};
            snprintf(modeString, sizeof(modeString), "%s data=8 parity=n stop=1 xon=off to=off odsr=off dtr=on rts=on",
                     baudToModeString(_baudRate));
            DCB portAttributes = {};
            portAttributes.DCBlength = sizeof(portAttributes);

            bool error = !BuildCommDCBA(modeString, &portAttributes);
            if (!error)
                error |= !SetCommState(_handle, &portAttributes);

            COMMTIMEOUTS timeouts = {};
            timeouts.ReadIntervalTimeout = MAXDWORD;
            if (!error)
                error |= !SetCommTimeouts(_handle, &timeouts);

            if(error) {
                CloseHandle(_handle);
                throw std::runtime_error("Could not reconfigure port.");
            }

            PurgeComm(_handle, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);
            _isOpen = true;
        }

        void close() final {
            if (_isOpen) {
                CloseHandle(_handle);
            }
            _isOpen = false;
        }

        void flush() final {  }

        int32_t readByte() final {
            int32_t n = 0;
            uint8_t byte;
            if (!_isOpen || !ReadFile(_handle, &byte, 1, reinterpret_cast<LPDWORD>(&n), NULL) || n != 1)
                return -1;
            return (int32_t) byte;
        }

        size_t read(uint8_t* buffer, size_t offset, size_t count) final {
            if (!_isOpen)
                return 0;
            int32_t n = 0;
            volatile bool error = !ReadFile(_handle, const_cast<unsigned char*>(&buffer[offset]), count, reinterpret_cast<LPDWORD>(&n), NULL);
            if (error)
                throw std::runtime_error("Error while reading from port.");
            return n;
        }

        void writeByte(uint8_t byte) final {
            int32_t bytesWritten;
            if (_isOpen) {
                WriteFile(_handle, &byte, 1, reinterpret_cast<LPDWORD>(&bytesWritten), NULL);
            }
        }

        void write(const uint8_t* buffer, size_t offset, size_t count) final {
            int32_t bytesWritten;
            if (_isOpen) {
                WriteFile(_handle, const_cast<unsigned char *>(&buffer[offset]), count, reinterpret_cast<LPDWORD>(&bytesWritten), NULL);
            }

        }

        static const char* baudToModeString(uint32_t baudrate) {
            switch(baudrate) {
                case     110 : return "baud=110";
                case     300 : return "baud=300";
                case     600 : return "baud=600";
                case    1200 : return "baud=1200";
                case    2400 : return "baud=2400";
                case    4800 : return "baud=4800";
                case    9600 : return "baud=9600";
                case   19200 : return "baud=19200";
                case   38400 : return "baud=38400";
                case   57600 : return "baud=57600";
                case  115200 : return "baud=115200";
                case  128000 : return "baud=128000";
                case  256000 : return "baud=256000";
                case  500000 : return "baud=500000";
                case  921600 : return "baud=921600";
                case 1000000 : return "baud=1000000";
                case 1500000 : return "baud=1500000";
                case 2000000 : return "baud=2000000";
                case 3000000 : return "baud=3000000";
                default:
                    throw std::invalid_argument("Baud rate not supported.");
            }
        }

    private:
        char _portName[16] = {0};
        HANDLE _handle;
        int32_t _baudRate;
        bool _isOpen;
    };

}}}

#endif
#endif //UC_CORE_WIN32SERIALPORT_H
