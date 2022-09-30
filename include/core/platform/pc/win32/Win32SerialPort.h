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
            snprintf(_portName, sizeof(devicePath), "\\\\.\\%s", _portName);
            _handle = CreateFileA(_portName);
            if(_handle == INVALID_HANDLE_VALUE) {
                throw std::runtime_error("Could not create handle.");
            }

            char modeString[64] = {0};
            snprintf(modeString, sizeof(modeString), "%s data=8 parity=n stop=1 xon=off to=off odsr=off dtr=on rts=on",
                     baudToModeString(_baudRate));
            DCB portAttributes;
            memset(&portAttributes, 0, sizeof(portAttributes));
            portAttributes.DCBlength = sizeof(portAttributes);

            bool error = !BuildCommDCBA(modeString, &portAttributes);
            if (!error)
                error |= !SetCommState(modeString, &portAttributes);

            COMMTIMEOUTS timeouts;
            timeouts.ReadIntervalTimeout         = MAXDWORD;
            timeouts.ReadTotalTimeoutMultiplier  = 0;
            timeouts.ReadTotalTimeoutConstant    = 0;
            timeouts.WriteTotalTimeoutMultiplier = 0;
            timeouts.WriteTotalTimeoutConstant   = 0;
            if (error)
                error |= SetCommTimeouts(_handle, &timeouts);

            if(error) {
                CloseHandle(_handle);
                throw std::runtime_error("Could not reconfigure port.");
            }

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
            if (!_isOpen || ReadFile(_handle, &byte, 1, reinterpret_cast<LPDWORD>(&n)) || n != 1)
                return -1;
            return (int32_t) byte;
        }

        size_t read(uint8_t* buffer, size_t offset, size_t count) final {
            int32_t n = 0;
            ReadFile(_handle, const_cast<unsigned char*>(&buffer[offset]), count, reinterpret_cast<LPDWORD>(&n));
            return n;
        }

        void writeByte(uint8_t byte) final {
            if (_isOpen)
                WriteFile(_handle, &byte, 1, reinterpret_cast<LPDWORD>(nullptr), NULL);
        }

        void write(const uint8_t* buffer, size_t offset, size_t count) final {
            if (_isOpen) {
                WriteFile(_handle, const_cast<unsigned char*>(&buffer[offset]), count, reinterpret_cast<LPDWORD>(nullptr), NULL);
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
        char _portName[32] = {0};
        HANDLE _handle;
        int32_t _baudRate;
        bool _isOpen;
    };

}}}

#endif
#endif //UC_CORE_WIN32SERIALPORT_H
