#pragma once
// esp32emu — Adafruit Si7021 Temperature & Humidity sensor mock

#include <Arduino.h>
#include <Wire.h>
#include <cstdint>

class Adafruit_Si7021 {
public:
    Adafruit_Si7021() {}

    bool begin(TwoWire* wire = nullptr) {
        (void)wire;
        _initialized = true;
        return true;
    }

    float readTemperature() { return _initialized ? _temperature : 0.0f; }
    float readHumidity() { return _initialized ? _humidity : 0.0f; }

    void reset() {
        _heater = false;
        _resolution = 0;
    }

    void heater(bool on) { _heater = on; }
    bool isHeaterEnabled() const { return _heater; }

    uint8_t getRevision() const { return 0x20; } // Si7021
    uint32_t sernum_a = 0x12345678;
    uint32_t sernum_b = 0xABCDEF00;

    void setResolution(uint8_t res) { _resolution = res; }
    uint8_t getResolution() const { return _resolution; }

    // --- Test helpers ---
    void test_setTemperature(float t) { _temperature = t; }
    void test_setHumidity(float h) { _humidity = h; }
    bool test_isInitialized() const { return _initialized; }

private:
    bool _initialized = false;
    float _temperature = 23.5f;
    float _humidity = 45.0f;
    bool _heater = false;
    uint8_t _resolution = 0;
};
