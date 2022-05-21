#ifndef HAL_SERIALPORT_H_
#define HAL_SERIALPORT_H_

#include <stdint.h>
#include <fsl_uart.h>
#include "clock_config.h"
#include "core/io/Stream.h"

namespace fslCore {
    class SerialPort : public core::io::Stream {
    public:

        explicit SerialPort(UART_Type *uart, clock_name_t clockSource = kCLOCK_CoreSysClk) : _uart(uart), _clockFreq(
                CLOCK_GetFreq(clockSource)) {
            UART_GetDefaultConfig(&_uartConfig);
            _uartConfig.baudRate_Bps = 9600;
        }

        ~SerialPort() {
            close();
        }

        void setBaudRate(uint32_t baudRate) {
            if (!_isOpen)
                _uartConfig.baudRate_Bps = baudRate;
        }

        bool canRead() const override { return true; }

        bool canWrite() const override { return true; }

        size_t getLength() const override { return 0; }

        size_t getPosition() const override { return 0; }

        void open() {
            if (!_isOpen) {
                UART_Init(_uart, &_uartConfig, _clockFreq);
                UART_EnableRx(_uart, true);
                UART_EnableTx(_uart, true);
                _isOpen = true;
            }
        }

        void close() override {
            if (_isOpen) {
                flush();
                UART_EnableRx(_uart, false);
                UART_EnableTx(_uart, false);
                UART_Deinit(_uart);
            }
        }

        void flush() override {
            while (!(_uart->S1 & UART_S1_TDRE_MASK));
        }

        int32_t readByte() override {
            if (_isOpen && any()) {
                return (int32_t) _uart->D;
            }
            return -1;
        }

        void writeByte(uint8_t byte) override {
            if (_isOpen) {
                UART_WriteByte(_uart, byte);
                flush();
            }
        }

    private:

        bool any() {
#if defined(FSL_FEATURE_UART_HAS_FIFO) && FSL_FEATURE_UART_HAS_FIFO
            return _uart->RCFIFO;
#else
            return ((_uart->S1 & UART_S1_RDRF_MASK));
#endif
        }

    private:
        UART_Type *_uart;
        uart_config_t _uartConfig;
        uint32_t _clockFreq;
        bool _isOpen = false;
        volatile bool _isSending;

    };
}
#endif
