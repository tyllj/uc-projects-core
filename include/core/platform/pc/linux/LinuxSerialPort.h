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
#include "core/Try.h"

namespace core { namespace platform { namespace pc {
    class LinuxSerialPort : public io::Stream {
    private:
        struct Finalizer {
            Finalizer(int handle) : _handle(handle) {}
            auto operator()() const -> void { ::close(_handle); /* also unlocks according to manpage. */ }
            int _handle;
        };

    public:
        static auto open(const char* portName, int32_t baudRate) -> core::ErrorOr<LinuxSerialPort> {
            if (access(portName, F_OK) != 0)
                return core::Error(0x0232FA11, "Port does not exist, or you don't have the rights to access it.");

            auto port = ::open(portName, O_RDWR | O_NOCTTY | O_NDELAY);
            if (port == -1)
                return core::Error(0x0232FA11, "Could not open port.");
            auto portFinalizer = core::Defer(Finalizer(port));

            if (flock(port, LOCK_EX|LOCK_NB) != 0)
                return core::Error(0x0232FA11, "Port is used by an other process");

            termios portAttributes;
            if (tcgetattr(port, &portAttributes) == -1)
                return core::Error(0x0232FA11, "Could not read current port settings.");

            // Hardcoded mode: 8 data bits, no parity, 1 stop bit
            int32_t portStatus;
            bool error = tcsetattr(port, TCSANOW, &portAttributes) == -1;
            portAttributes.c_cflag = CS8 | CLOCAL | CREAD;
            portAttributes.c_iflag = IGNPAR;
            portAttributes.c_oflag = 0;
            portAttributes.c_lflag = 0;
            portAttributes.c_cc[VMIN] = 0;      /* block until n bytes are received */
            portAttributes.c_cc[VTIME] = 0;     /* block until a timer expires (n * 100 mSec.) */
            cfsetispeed(&portAttributes, TRY(baudToSpeed(baudRate)));
            cfsetospeed(&portAttributes, TRY(baudToSpeed(baudRate)));

            if(!error)
                error |= ioctl(port, TIOCMGET, &portStatus) == -1;
            portStatus |= TIOCM_DTR;    /* turn on DTR */
            portStatus |= TIOCM_RTS;    /* turn on RTS */
            if(!error)
                error |= ioctl(port, TIOCMSET, &portStatus) == -1;

            if (error)
                return core::Error(0x0232FA11, "Could not reconfigure port.");

            return LinuxSerialPort(port, etl::move(portFinalizer));
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
        explicit LinuxSerialPort(int handle, core::Defer<Finalizer>&& finalizer) :  _handle(handle), _finalizer(std::move(finalizer)) { }

        static auto baudToSpeed(uint32_t baudrate) -> core::ErrorOr<speed_t> {
            switch(baudrate) {
                case 50 : return speed_t(B50);
                case 75 : return speed_t(B75);
                case 110 : return speed_t(B110);
                case 134 : return speed_t(B134);
                case 150 : return speed_t(B150);
                case 200 : return speed_t(B200);
                case 300 : return speed_t(B300);
                case 600 : return speed_t(B600);
                case 1200 : return speed_t(B1200);
                case 1800 : return speed_t(B1800);
                case 2400 : return speed_t(B2400);
                case 4800 : return speed_t(B4800);
                case 9600 : return speed_t(B9600);
                case 19200 : return speed_t(B19200);
                case 38400 : return speed_t(B38400);
                case 57600 : return speed_t(B57600);
                case 115200 : return speed_t(B115200);
                case 230400 : return speed_t(B230400);
                case 460800 : return speed_t(B460800);
                case 500000 : return speed_t(B500000);
                case 576000 : return speed_t(B576000);
                case 921600 : return speed_t(B921600);
                case 1000000 : return speed_t(B1000000);
                case 1152000 : return speed_t(B1152000);
                case 1500000 : return speed_t(B1500000);
                case 2000000 : return speed_t(B2000000);
                case 2500000 : return speed_t(B2500000);
                case 3000000 : return speed_t(B3000000);
                case 3500000 : return speed_t(B3500000);
                case 4000000 : return speed_t(B4000000);
            }
            return core::InvalidArgumentError("Baudrate not supported.");
        }

    private:
        int32_t _handle;
        core::Defer<Finalizer> _finalizer;
    };
}}}

#endif
#endif //UC_CORE_LINUXSERIALPORT_H
