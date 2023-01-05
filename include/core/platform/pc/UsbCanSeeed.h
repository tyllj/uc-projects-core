//
// Created by tyll on 2022-05-01.
//

#ifndef UC_CORE_USBCANSEEED_H
#define UC_CORE_USBCANSEEED_H

#include <stdint.h>
#include "core/Math.h"

#include "core/platform/pc/Usb.h"
#include "core/io/Stream.h"
#include "core/shared_ptr.h"
#include "core/can/ICanInterface.h"
#include "core/Bits.h"
#include "SerialPort.h"

namespace core { namespace can {
    constexpr int32_t USBCAN_SERIAL_BAUD = 2000000;

    class UsbCanSeeed : public ICanInterface {
    private:
        typedef enum {
            CANUSB_SPEED_INVALID = 0x00,
            CANUSB_SPEED_1000000 = 0x01,
            CANUSB_SPEED_800000  = 0x02,
            CANUSB_SPEED_500000  = 0x03,
            CANUSB_SPEED_400000  = 0x04,
            CANUSB_SPEED_250000  = 0x05,
            CANUSB_SPEED_200000  = 0x06,
            CANUSB_SPEED_125000  = 0x07,
            CANUSB_SPEED_100000  = 0x08,
            CANUSB_SPEED_50000   = 0x09,
            CANUSB_SPEED_20000   = 0x0a,
            CANUSB_SPEED_10000   = 0x0b,
            CANUSB_SPEED_5000    = 0x0c,
        } CANUSB_SPEED;

        typedef enum {
            CANUSB_MODE_NORMAL          = 0x00,
            CANUSB_MODE_LOOPBACK        = 0x01,
            CANUSB_MODE_SILENT          = 0x02,
            CANUSB_MODE_LOOPBACK_SILENT = 0x03,
        } CANUSB_MODE;

        typedef enum {
            CANUSB_FRAME_STANDARD = 0x01,
            CANUSB_FRAME_EXTENDED = 0x02,
        } CANUSB_FRAME;

    public:
        UsbCanSeeed(core::io::Stream& serialPort, uint32_t baudRate = 500000) :
            _serialPort(serialPort),
            _filter(0x00),
            _mask(0x7FF),
            _speed(canusb_int_to_speed(baudRate)) {
            while(serialPort.readByte() != -1);
            init();
        }

        auto filter(canid_t filter) -> void final {
            if (_filter == filter)
                return;
            _filter = filter;
            init();
        }

        auto mask(canid_t mask) -> void final {
            if (_mask == mask)
                return;
            _mask = mask;
            init();
        }

        auto writeFrame(const CanFrame &canFrame) -> void final {
            CANUSB_FRAME frame = CANUSB_FRAME_STANDARD;
            unsigned char id_lsb = canFrame.id & 0xFF;
            unsigned char id_msb = (canFrame.id >> 8) & 0x07;
            const unsigned char* data = canFrame.payload;
            int data_length_code = canFrame.length;
#define MAX_FRAME_SIZE 13
            int data_frame_len = 0;
            unsigned char data_frame[MAX_FRAME_SIZE] = {0x00};

            /* Byte 0: Packet Start */
            data_frame[data_frame_len++] = 0xaa;

            /* Byte 1: CAN Bus Data Frame Information */
            data_frame[data_frame_len] = 0x00;
            data_frame[data_frame_len] |= 0xC0; /* Bit 7 Always 1, Bit 6 Always 1 */
            if (frame == CANUSB_FRAME_STANDARD)
                data_frame[data_frame_len] &= 0xDF; /* STD frame */
            else /* CANUSB_FRAME_EXTENDED */
                data_frame[data_frame_len] |= 0x20; /* EXT frame */
            data_frame[data_frame_len] &= 0xEF; /* 0=Data */
            data_frame[data_frame_len] |= data_length_code; /* DLC=data_len */
            data_frame_len++;

            /* Byte 2 to 3: ID */
            data_frame[data_frame_len++] = id_lsb; /* lsb */
            data_frame[data_frame_len++] = id_msb; /* msb */

            /* Byte 4 to (4+data_len): Data */
            for (int i = 0; i < data_length_code; i++)
                data_frame[data_frame_len++] = data[i];

            /* Last byte: End of frame */
            data_frame[data_frame_len++] = 0x55;
            enforceInitTimegap();
            frame_send(data_frame, data_frame_len);
        }

        bool tryReadFrame(CanFrame& outCanFrame) final {
            unsigned char byte;
            bool frame_completed { false };

            while (_serialPort.read(&byte, 1) == 1) {
                _serialFrame[_serialFrameLength++] = byte;
                frame_completed = frame_is_complete(_serialFrame, _serialFrameLength);
                if (frame_completed)
                    break;
            }

            if (frame_completed && (_serialFrameLength == 20) && (_serialFrame[0] == 0xaa) && (_serialFrame[1] == 0x55)) {
                _serialFrameLength = 0;
                return false; // ignore command frames;
            }

            if (frame_completed && (_serialFrame[0] == 0xaa)) {
                CanFrame& frame = outCanFrame;
                frame.id = _serialFrame[2] | (_serialFrame[3] << 8);
                frame.isRemoteRequest = bitRead(_serialFrame[1], 4);
                frame.length = _serialFrame[1] & 0x0F;
                memset(frame.payload, 0x00, 8);
                memcpy(frame.payload, &_serialFrame[4], frame.length);

                _serialFrameLength = 0;
                return true;
            }
            return false;
        }

    private:
        auto frame_send(const unsigned char *frame, int frame_len) -> int {
            _serialPort.write((const uint8_t*)(frame), 0, frame_len);
            return frame_len;
        }

        auto enforceInitTimegap() -> void {
            const uint64_t timegap = 50;
            uint64_t millisSinceInit = core::millisPassedSince(_lastInit);
            if (millisSinceInit < timegap)
                core::sleepms(timegap - millisSinceInit);
        }

        void init() {
            enforceInitTimegap();
            int cmd_frame_len;
            unsigned char cmd_frame[20];
            CANUSB_MODE mode = CANUSB_MODE_NORMAL;
            unsigned char filter_lsb = _filter & 0xFF;
            unsigned char filter_msb = (_filter >> 8) & 0x07;
            unsigned char mask_lsb = _mask & 0xFF;
            unsigned char mask_msb = (_mask >> 8) & 0x07;

            cmd_frame_len = 0;
            cmd_frame[cmd_frame_len++] = 0xaa;
            cmd_frame[cmd_frame_len++] = 0x55;
            cmd_frame[cmd_frame_len++] = 0x12;
            cmd_frame[cmd_frame_len++] = _speed;
            cmd_frame[cmd_frame_len++] = CANUSB_FRAME_STANDARD;
            cmd_frame[cmd_frame_len++] = filter_lsb;
            cmd_frame[cmd_frame_len++] = filter_msb;
            cmd_frame[cmd_frame_len++] = 0; /* Filter ID 29-bit not handled. */
            cmd_frame[cmd_frame_len++] = 0; /* Filter ID 29-bit not handled. */
            cmd_frame[cmd_frame_len++] = mask_lsb;
            cmd_frame[cmd_frame_len++] = mask_msb;
            cmd_frame[cmd_frame_len++] = 0; /* Mask ID 29-bit not handled. */
            cmd_frame[cmd_frame_len++] = 0; /* Mask ID 29-bit not handled. */
            cmd_frame[cmd_frame_len++] = mode;
            cmd_frame[cmd_frame_len++] = 0; // Reference impl says this should be 01, but USBCAN app sends 00 here.
            cmd_frame[cmd_frame_len++] = 0;
            cmd_frame[cmd_frame_len++] = 0;
            cmd_frame[cmd_frame_len++] = 0;
            cmd_frame[cmd_frame_len++] = 0;
            cmd_frame[cmd_frame_len++] = generate_checksum(&cmd_frame[2], 17);

            _serialPort.write(cmd_frame, 0, cmd_frame_len);

            _lastInit = core::millis();
        }

        static auto canusb_int_to_speed(int speed) -> CANUSB_SPEED {
            switch (speed) {
                case 1000000:
                    return CANUSB_SPEED_1000000;
                case 800000:
                    return CANUSB_SPEED_800000;
                case 500000:
                    return CANUSB_SPEED_500000;
                case 400000:
                    return CANUSB_SPEED_400000;
                case 250000:
                    return CANUSB_SPEED_250000;
                case 200000:
                    return CANUSB_SPEED_200000;
                case 125000:
                    return CANUSB_SPEED_125000;
                case 100000:
                    return CANUSB_SPEED_100000;
                case 50000:
                    return CANUSB_SPEED_50000;
                case 20000:
                    return CANUSB_SPEED_20000;
                case 10000:
                    return CANUSB_SPEED_10000;
                case 5000:
                    return CANUSB_SPEED_5000;
                default:
                    return CANUSB_SPEED_INVALID;
            }
        }

        static auto generate_checksum(const unsigned char *data, int data_len) -> int {
            int i, checksum;

            checksum = 0;
            for (i = 0; i < data_len; i++) {
                checksum += data[i];
            }

            return checksum & 0xff;
        }

        static auto frame_is_complete(const unsigned char *frame, int frame_len) -> bool {
            if (frame_len > 0) {
                if (frame[0] != 0xaa) {
                    /* Need to sync on 0xaa at start of frames, so just skip. */
                    return true;
                }
            }

            if (frame_len < 2) {
                return false;
            }

            if (frame[1] == 0x55) { /* Command frame... */
                if (frame_len >= 20) { /* ...always 20 bytes. */
                    return true;
                } else {
                    return false;
                }
            } else if ((frame[1] >> 4) == 0xc) { /* Data frame... */
                if (frame_len >= (frame[1] & 0xf) + 5) { /* ...payload and 5 bytes. */
                    return true;
                } else {
                    return false;
                }
            }

            /* Unhandled frame type. */
            return true;
        }

    private:
        core::io::Stream& _serialPort;
        canid_t _filter;
        canid_t _mask;
        CANUSB_SPEED _speed;
        uint8_t _serialFrame[32] = {};
        uint8_t _serialFrameLength = 0;
        uint64_t _lastInit = 0;
    };


}}

#endif //UC_CORE_USBCANSEEED_H
