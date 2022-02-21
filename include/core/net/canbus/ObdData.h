//
// Created by tyll on 06.02.22.
//

#ifndef FIRMWARE_OBDDATA_H
#define FIRMWARE_OBDDATA_H

#include <stdint.h>
namespace core { namespace net { namespace canbus {
    struct ObdValue {
    public:
        inline explicit ObdValue(const uint32_t rawValue) : _rawInt(rawValue) {}

        inline ObdValue(const uint8_t a, const uint8_t b, const uint8_t c, const uint8_t d) : ObdValue(
                ((uint32_t) a << 24 | (uint32_t) b << 16 | (uint32_t) c << 8 | (uint32_t) d)) {}

        inline uint16_t asWord() const { return _rawInt >> 16; }

        inline uint32_t asRaw() const { return _rawInt; }

        inline uint8_t A() const { return _rawInt >> 24; }

        inline uint8_t B() const { return _rawInt >> 16; }

        inline uint8_t C() const { return _rawInt >> 8; }

        inline uint8_t D() const { return _rawInt >> 0; }

    private:
        const uint32_t _rawInt;
    };

    struct PidRequest {
        uint8_t pid;
        uint8_t responseLength;
    };

    struct MultiResponse {
    private:
        uint32_t _values[6];
        uint8_t _pids[6];
        uint8_t _length;
    public:
        void add(uint8_t pid, ObdValue value) {
            _pids[_length] = pid;
            _values[_length] = value.asRaw();
            _length++;
        }

        void clear() {
            _length = 0;
        }

        const bool containsPid(uint8_t pid) const {
            for (uint8_t i = 0; i < _length; i++)
                if (_pids[i] == pid)
                    return true;
            return false;
        }

        const uint8_t pidAt(uint8_t i) const {
            return _pids[i];
        }

        const uint32_t valueAt(uint32_t i) const {
            return _values[i];
        }

        const ObdValue valueOf(uint8_t pid) const {
            for (uint8_t i = 0; i < _length; i++)
                if (_pids[i] == pid)
                    return ObdValue(_values[i]);

            return ObdValue(0);
        }

        const uint8_t length() const {
            return _length;
        }
    };

    class MultiRequest {
    private:
        PidRequest _pids[6] = {0};
        uint8_t _length = 0;
        uint8_t _dataLength = 0;
    public:
        void add(PidRequest pidRequest) {
            _pids[_length] = pidRequest;
            _length++;
            _dataLength += pidRequest.responseLength;
        }

        const PidRequest &at(uint8_t index) const {
            return _pids[index];
        }

        const PidRequest &pid(uint8_t pid) const {
            for (uint8_t i = 0; i < _length; i++)
                if (_pids[i].pid == pid)
                    return _pids[i];
            return *((PidRequest *) nullptr);
        }

        const bool containsPid(uint8_t pid) const {
            for (uint8_t i = 0; i < _length; i++)
                if (_pids[i].pid == pid)
                    return true;
            return false;
        }

        const uint8_t length() const {
            return _length;
        }

        const uint8_t dataLength() const {
            return _dataLength;
        }
    };
}}}

#endif //FIRMWARE_OBDDATA_H
