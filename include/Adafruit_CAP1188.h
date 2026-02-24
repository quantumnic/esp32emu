#pragma once
// esp32emu — Adafruit CAP1188 8-channel Capacitive Touch mock

#include <Arduino.h>
#include <Wire.h>
#include <cstdint>

class Adafruit_CAP1188 {
public:
    Adafruit_CAP1188(int8_t resetPin = -1) {
        (void)resetPin;
    }

    bool begin(uint8_t addr = 0x29, TwoWire* wire = nullptr) {
        (void)wire;
        _address = addr;
        _initialized = true;
        return true;
    }

    // Read touched channels as 8-bit bitmask
    uint8_t touched() const {
        return _touched;
    }

    // Read individual channel delta (capacitance change)
    int8_t readDelta(uint8_t channel) const {
        if (channel >= 8) return 0;
        return _deltas[channel];
    }

    // Get/set sensitivity (1-128, powers of 2)
    void setSensitivity(uint8_t sensitivity) { _sensitivity = sensitivity; }
    uint8_t getSensitivity() const { return _sensitivity; }

    // Enable/disable multi-touch
    void setMultiTouch(bool enable) { _multiTouch = enable; }

    // Get product ID (should be 0x50 for CAP1188)
    uint8_t readProductID() const { return 0x50; }

    // Get manufacturer ID (should be 0x5D)
    uint8_t readManufacturerID() const { return 0x5D; }

    // Get revision
    uint8_t readRevision() const { return 0x83; }

    // --- Test helpers ---
    void test_setTouched(uint8_t mask) { _touched = mask; }
    void test_setDelta(uint8_t channel, int8_t delta) {
        if (channel < 8) _deltas[channel] = delta;
    }

private:
    uint8_t _address = 0x29;
    bool _initialized = false;
    bool _multiTouch = false;
    uint8_t _sensitivity = 32;
    uint8_t _touched = 0x00;
    int8_t _deltas[8] = {};
};
