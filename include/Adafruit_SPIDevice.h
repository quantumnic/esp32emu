#pragma once
#include <cstdint>
#include <cstring>
#include <cstdio>

class Adafruit_SPIDevice {
public:
    Adafruit_SPIDevice(int8_t cs, uint32_t freq = 1000000,
                       uint8_t bitOrder = 0, uint8_t dataMode = 0,
                       void* spi = nullptr)
        : _cs(cs), _freq(freq), _bitOrder(bitOrder), _dataMode(dataMode), _spi(spi) {}

    // Software SPI constructor
    Adafruit_SPIDevice(int8_t cs, int8_t sck, int8_t miso, int8_t mosi,
                       uint32_t freq = 1000000, uint8_t bitOrder = 0,
                       uint8_t dataMode = 0)
        : _cs(cs), _freq(freq), _bitOrder(bitOrder), _dataMode(dataMode),
          _sck(sck), _miso(miso), _mosi(mosi) {}

    bool begin() { _begun = true; return true; }
    bool detected() const { return _begun; }

    bool read(uint8_t* buffer, size_t len, uint8_t sendvalue = 0xFF) {
        (void)sendvalue;
        if (!_begun || !buffer) return false;
        size_t copyLen = (len < _rxLen) ? len : _rxLen;
        memcpy(buffer, _rxBuf, copyLen);
        return true;
    }

    bool write(const uint8_t* buffer, size_t len,
               const uint8_t* prefix = nullptr, size_t prefix_len = 0) {
        if (!_begun) return false;
        _txLen = 0;
        if (prefix && prefix_len > 0) {
            size_t pLen = (prefix_len < sizeof(_txBuf)) ? prefix_len : sizeof(_txBuf);
            memcpy(_txBuf, prefix, pLen);
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
                         uint8_t sendvalue = 0xFF) {
        (void)sendvalue;
        if (!write(write_buffer, write_len)) return false;
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
    int8_t _cs;
    uint32_t _freq;
    uint8_t _bitOrder;
    uint8_t _dataMode;
    void* _spi = nullptr;
    int8_t _sck = -1, _miso = -1, _mosi = -1;
    bool _begun = false;
    uint8_t _rxBuf[256] = {};
    size_t _rxLen = 0;
    uint8_t _txBuf[256] = {};
    size_t _txLen = 0;
};
