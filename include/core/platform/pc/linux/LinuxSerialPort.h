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
#include "core/Defer.h"

namespace core { namespace platform { namespace pc {
    class LinuxSerialPort : public io::Stream {
    private:
        struct Finalizer {
            Finalizer(int handle) : _handle(handle) {}
            auto operator()() -> void { ::close(_handle); /* also unlocks according to manpage. */ }
            int _handle;
        };

    public:
        static auto open(const char* portName, int32_t baudRate) -> core::ErrorOr<LinuxSerialPort> {
            if (access(portName, F_OK) != 0)
                return core::Error(0x0232FA11, "Port does not exist, or you don't have the rights to access it.");

            auto port = ::open(portName, O_RDWR | O_NOCTTY | O_NDELAY);
            if (port == -1)
                return core::Error(0x0232FA11, "Could not open port.");
            auto portFinalizer = core::Defer(Finalizer(portname));

            if (flock(port, LOCK_EX|LOCK_NB) != 0)
                return core::Error(0x0232FA11, "Port is used by an other process");

            termios portAttributes;
            if (tcgetattr(_handle, &portAttributes) == -1)
                return core::Error(0x0232FA11, "Could not read current port settings.");

            // Hardcoded mode: 8 data bits, no parity, 1 stop bit
            int32_t portStatus;
            bool error = tcsetattr(_handle, TCSANOW, &portAttributes) == -1;
            portAttributes.c_cflag = CS8 | CLOCAL | CREAD;
            portAttributes.c_iflag = IGNPAR;
            portAttributes.c_oflag = 0;
            portAttributes.c_lflag = 0;
            portAttributes.c_cc[VMIN] = 0;      /* block until n bytes are received */
            portAttributes.c_cc[VTIME] = 0;     /* block until a timer expires (n * 100 mSec.) */
            cfsetispeed(&portAttributes, TRY(baudToSpeed(_baudRate)));
            cfsetospeed(&portAttributes, TRY(baudToSpeed(_baudRate)));

            if(!error)
                error |= ioctl(_handle, TIOCMGET, &portStatus) == -1;
            portStatus |= TIOCM_DTR;    /* turn on DTR */
            portStatus |= TIOCM_RTS;    /* turn on RTS */
            if(!error)
                error |= ioctl(_handle, TIOCMSET, &portStatus) == -1;

            if (error)
                return core::Error(0x0232FA11, "Could not reconfigure port.");

            return LinuxSerialPort(port, portFinalizer);
        }


        bool canRead() const final { return true; }
        bool canWrite() const final { return true; }
        size_t length() const final { return 0; }
        size_t position() const final { return 0; }

        void close() final {  }

        void flush() final {  }

        int32_t readByte() final {
            uint8_t byte;
            if (::read(_handle, &byte, 1) != 1)
                return -1;
            return (int32_t) byte;
        }

        size_t read(uint8_t* buffer, size_t offset, size_t count) final {
            return ::read(_handle, &buffer[offset], count);
        }

        void writeByte(uint8_t byte) final {
            ::write(_handle, &byte, 1);
        }

        void write(const uint8_t* buffer, size_t offset, size_t count) final {
            ::write(_handle, const_cast<unsigned char*>(&buffer[offset]), count);
        }
    private:
        explicit LinuxSerialPort(int handle, core::Defer<Finalizer>&& finalizer) :  _port(handle), _finalizer(std::move(finalizer)) { }

        static baudToSpeed(uint32_t baudrate) const -> core::ErrorOr<speed_t> {
            switch(baudrate) {
                case 50 : return B50;
                case 75 : return B75;
                case 110 : return B110;
                case 134 : return B134;
                case 150 : return B150;
                case 200 : return B200;
                case 300 : return B300;
                case 600 : return B600;
                case 1200 : return B1200;
                case 1800 : return B1800;
                case 2400 : return B2400;
                case 4800 : return B4800;
                case 9600 : return B9600;
                case 19200 : return B19200;
                case 38400 : return B38400;
                case 57600 : return B57600;
                case 115200 : return B115200;
                case 230400 : return B230400;
                case 460800 : return B460800;
                case 500000 : return B500000;
                case 576000 : return B576000;
                case 921600 : return B921600;
                case 1000000 : return B1000000;
                case 1152000 : return B1152000;
                case 1500000 : return B1500000;
                case 2000000 : return B2000000;
                case 2500000 : return B2500000;
                case 3000000 : return B3000000;
                case 3500000 : return B3500000;
                case 4000000 : return B4000000;
                default:
                    return core::InvalidArgumentError("Baudrate not supported.");
            }
        }

    private:
        int32_t _handle;
        core::Defer<Finalizer> _finalizer;
    };
}}}

#endif
#endif //UC_CORE_LINUXSERIALPORT_H
