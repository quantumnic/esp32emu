#pragma once
// esp32emu: BH1750 ambient light sensor mock
#include <cstdint>
#include <cstdio>

class BH1750 {
public:
    enum Mode {
        CONTINUOUS_HIGH_RES_MODE   = 0x10,
        CONTINUOUS_HIGH_RES_MODE_2 = 0x11,
        CONTINUOUS_LOW_RES_MODE    = 0x13,
        ONE_TIME_HIGH_RES_MODE     = 0x20,
        ONE_TIME_HIGH_RES_MODE_2   = 0x21,
        ONE_TIME_LOW_RES_MODE      = 0x23
    };

    BH1750(uint8_t addr = 0x23) : _addr(addr), _begun(false), _mode(CONTINUOUS_HIGH_RES_MODE), _lux(0.0f) {}

    bool begin(Mode mode = CONTINUOUS_HIGH_RES_MODE) {
        _mode = mode;
        _begun = true;
        printf("[esp32emu] BH1750: begin (addr=0x%02X, mode=0x%02X)\n", _addr, _mode);
        return true;
    }

    bool configure(Mode mode) {
        _mode = mode;
        return true;
    }

    bool measurementReady(bool /*maxWait*/ = false) { return _begun; }

    float readLightLevel() {
        if (!_begun) return -1.0f;
        return _lux;
    }

    // Test helpers
    void setLux(float lux) { _lux = lux; }
    float getLux() const { return _lux; }
    bool isBegun() const { return _begun; }
    uint8_t getAddress() const { return _addr; }
    Mode getMode() const { return _mode; }

private:
    uint8_t _addr;
    bool _begun;
    Mode _mode;
    float _lux;
};
