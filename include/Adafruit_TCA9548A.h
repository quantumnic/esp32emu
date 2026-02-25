// esp32emu — Adafruit_TCA9548A mock: I2C multiplexer (8 channels)
#pragma once
#include <cstdint>
#include <cstdio>

class Adafruit_TCA9548A {
public:
    Adafruit_TCA9548A(uint8_t addr = 0x70) : _addr(addr), _begun(false), _channel(0xFF) {}

    bool begin(uint8_t addr = 0xFF, void* wire = nullptr) {
        (void)wire;
        if (addr != 0xFF) _addr = addr;
        _begun = true;
        _channel = 0xFF;
        printf("[esp32emu] TCA9548A: initialized (I2C 0x%02X)\n", _addr);
        return true;
    }

    void selectChannel(uint8_t ch) {
        if (ch > 7) return;
        _channel = ch;
        printf("[esp32emu] TCA9548A: selected channel %u\n", ch);
    }

    uint8_t getChannel() const { return _channel; }

    void disableAll() {
        _channel = 0xFF;
        printf("[esp32emu] TCA9548A: all channels disabled\n");
    }

    // --- Test helpers ---
    bool test_isBegun() const { return _begun; }
    uint8_t test_getChannel() const { return _channel; }
    uint8_t test_getAddr() const { return _addr; }

private:
    uint8_t _addr;
    bool _begun;
    uint8_t _channel;
};
