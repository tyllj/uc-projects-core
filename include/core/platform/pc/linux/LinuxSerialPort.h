//
// Created by tyll on 29.09.22.
//

#ifndef UC_CORE_LINUXSERIALPORT_H
#define UC_CORE_LINUXSERIALPORT_H
#if defined(__linux__)

#include "core/io/Stream.h"

#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <sys/file.h>
#include <errno.h>
#include <string.h>
#include <stdexcept>

namespace core { namespace platform { namespace pc {
    class LinuxSerialPort : public io::Stream {
    public:
        explicit LinuxSerialPort(const char* portName) :  _baudRate(9600), _isOpen(false) {
            strncpy(_portName, portName, sizeof(_portName));
        }

        ~LinuxSerialPort() {
            LinuxSerialPort::close();
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

            if (access(_portName, F_OK) != 0)
                throw std::runtime_error("Port does not exist, or you don't have the rights to access it.");

            _handle = ::open(_portName, O_RDWR | O_NOCTTY | O_NDELAY);
            if (_handle == -1)
                throw std::runtime_error("Could not open port.");

            if (flock(_handle, LOCK_EX|LOCK_NB) != 0) {
                ::close(_handle);
                throw std::runtime_error("Port is used by an other process");
            }

            termios portAttributes;
            if (tcgetattr(_handle, &portAttributes) == -1) {
                ::close(_handle);
                flock(_handle, LOCK_UN);
                throw std::runtime_error("Could not read current port settings.");
            }

            // Hardcoded mode: 8 data bits, no parity, 1 stop bit
            int32_t portStatus;
            bool error = tcsetattr(_handle, TCSANOW, &portAttributes) == -1;
            portAttributes.c_cflag = CS8 | CLOCAL | CREAD;
            portAttributes.c_iflag = IGNPAR;
            portAttributes.c_oflag = 0;
            portAttributes.c_lflag = 0;
            portAttributes.c_cc[VMIN] = 0;      /* block untill n bytes are received */
            portAttributes.c_cc[VTIME] = 0;     /* block untill a timer expires (n * 100 mSec.) */
            cfsetispeed(&portAttributes, baudToSpeed(_baudRate));
            cfsetospeed(&portAttributes, baudToSpeed(_baudRate));

            if(!error)
                error |= ioctl(_handle, TIOCMGET, &portStatus) == -1;
            portStatus |= TIOCM_DTR;    /* turn on DTR */
            portStatus |= TIOCM_RTS;    /* turn on RTS */
            if(!error)
                error |= ioctl(_handle, TIOCMSET, &portStatus) == -1;

            if (error) {
                ::close(_handle);
                flock(_handle, LOCK_UN);
                throw std::runtime_error("Could not reconfigure port.");
            }
            _isOpen = true;
        }

        void close() final {
            if (_isOpen) {
                ::close(_handle);
                flock(_handle, LOCK_UN);
            }
            _isOpen = false;
        }

        void flush() final {  }

        int32_t readByte() final {
            uint8_t byte;
            if (!_isOpen || ::read(_handle, &byte, 1) != 1)
                return -1;
            return (int32_t) byte;
        }

        size_t read(uint8_t* buffer, size_t offset, size_t count) final {
            return ::read(_handle, &buffer[offset], count);
        }

        void writeByte(uint8_t byte) final {
            if (_isOpen)
                ::write(_handle, &byte, 1);
        }

        void write(const uint8_t* buffer, size_t offset, size_t count) final {
            if (_isOpen)
                ::write(_handle, const_cast<unsigned char*>(&buffer[offset]), count);
        }

        static speed_t baudToSpeed(uint32_t baudrate) {
            switch(baudrate) {
                case 50 :
                    return B50;
                case 75 :
                    return B75;
                case 110 :
                    return B110;
                case 134 :
                    return B134;
                case 150 :
                    return B150;
                case 200 :
                    return B200;
                case 300 :
                    return B300;
                case 600 :
                    return B600;
                case 1200 :
                    return B1200;
                case 1800 :
                    return B1800;
                case 2400 :
                    return B2400;
                case 4800 :
                    return B4800;
                case 9600 :
                    return B9600;
                case 19200 :
                    return B19200;
                case 38400 :
                    return B38400;
                case 57600 :
                    return B57600;
                case 115200 :
                    return B115200;
                case 230400 :
                    return B230400;
                case 460800 :
                    return B460800;
                case 500000 :
                    return B500000;
                case 576000 :
                    return B576000;
                case 921600 :
                    return B921600;
                case 1000000 :
                    return B1000000;
                case 1152000 :
                    return B1152000;
                case 1500000 :
                    return B1500000;
                case 2000000 :
                    return B2000000;
                case 2500000 :
                    return B2500000;
                case 3000000 :
                    return B3000000;
                case 3500000 :
                    return B3500000;
                case 4000000 :
                    return B4000000;
                default:
                    throw std::invalid_argument("Baud rate not supported.");
            }
        }

    private:
        char _portName[32] = {0};
        int32_t _handle;
        int32_t _baudRate;
        bool _isOpen;
    };
}}}

#endif
#endif //UC_CORE_LINUXSERIALPORT_H
