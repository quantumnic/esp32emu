#pragma once
#include <cstdint>
#include <cstring>
#include <cstdio>

class Adafruit_I2CDevice {
public:
    Adafruit_I2CDevice(uint8_t addr, void* wire = nullptr)
        : _addr(addr), _wire(wire) {}

    bool begin(bool addr_detect = true) {
        (void)addr_detect;
        _begun = true;
        return true;
    }

    bool detected() const { return _begun; }
    uint8_t address() const { return _addr; }

    bool read(uint8_t* buffer, size_t len, bool stop = true) {
        (void)stop;
        if (!_begun || !buffer) return false;
        size_t copyLen = (len < _rxLen) ? len : _rxLen;
        memcpy(buffer, _rxBuf, copyLen);
        return true;
    }

    bool write(const uint8_t* buffer, size_t len, bool stop = true,
               const uint8_t* prefix_buffer = nullptr, size_t prefix_len = 0) {
        (void)stop;
        if (!_begun) return false;
        _txLen = 0;
        if (prefix_buffer && prefix_len > 0) {
            size_t pLen = (prefix_len < sizeof(_txBuf)) ? prefix_len : sizeof(_txBuf);
            memcpy(_txBuf, prefix_buffer, pLen);
            _txLen = pLen;
        }
        if (buffer && len > 0) {
            size_t space = sizeof(_txBuf) - _txLen;
            size_t copyLen = (len < space) ? len : space;
            memcpy(_txBuf + _txLen, buffer, copyLen);
            _txLen += copyLen;
        }
        return true;
    }

    bool write_then_read(const uint8_t* write_buffer, size_t write_len,
                         uint8_t* read_buffer, size_t read_len,
                         bool stop = false) {
        if (!write(write_buffer, write_len, stop)) return false;
        return read(read_buffer, read_len);
    }

    // Test helpers
    void setReadData(const uint8_t* data, size_t len) {
        size_t copyLen = (len < sizeof(_rxBuf)) ? len : sizeof(_rxBuf);
        memcpy(_rxBuf, data, copyLen);
        _rxLen = copyLen;
    }
    const uint8_t* getWrittenData() const { return _txBuf; }
    size_t getWrittenLen() const { return _txLen; }

private:
    uint8_t _addr;
    void* _wire;
    bool _begun = false;
    uint8_t _rxBuf[256] = {};
    size_t _rxLen = 0;
    uint8_t _txBuf[256] = {};
    size_t _txLen = 0;
};
