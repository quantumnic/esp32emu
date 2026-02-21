#pragma once
// esp32emu — Adafruit MLX90614 IR Thermometer mock

#include <Arduino.h>
#include <Wire.h>
#include <cstdint>

class Adafruit_MLX90614 {
public:
    Adafruit_MLX90614(uint8_t addr = 0x5A) : _addr(addr) {}

    bool begin(TwoWire* wire = nullptr) {
        (void)wire;
        _initialized = true;
        return true;
    }

    double readObjectTempC() { return _initialized ? _objectTemp : 0.0; }
    double readAmbientTempC() { return _initialized ? _ambientTemp : 0.0; }
    double readObjectTempF() { return _initialized ? _objectTemp * 9.0 / 5.0 + 32.0 : 0.0; }
    double readAmbientTempF() { return _initialized ? _ambientTemp * 9.0 / 5.0 + 32.0 : 0.0; }

    uint16_t readEmissivity() { return _emissivity; }
    void writeEmissivity(uint16_t e) { _emissivity = e; }

    // --- Test helpers ---
    void test_setObjectTemp(double t) { _objectTemp = t; }
    void test_setAmbientTemp(double t) { _ambientTemp = t; }
    bool test_isInitialized() const { return _initialized; }

private:
    uint8_t _addr;
    bool _initialized = false;
    double _objectTemp = 36.5;
    double _ambientTemp = 22.0;
    uint16_t _emissivity = 0xFFFF;
};
