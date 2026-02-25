#pragma once
// esp32emu — Adafruit MAX31856 Precision Thermocouple mock

#include <Arduino.h>
#include <cstdint>

enum max31856_thermocoupletype_t {
    MAX31856_TCTYPE_B = 0,
    MAX31856_TCTYPE_E = 1,
    MAX31856_TCTYPE_J = 2,
    MAX31856_TCTYPE_K = 3,
    MAX31856_TCTYPE_N = 4,
    MAX31856_TCTYPE_R = 5,
    MAX31856_TCTYPE_S = 6,
    MAX31856_TCTYPE_T = 7,
};

enum max31856_conversionmode_t {
    MAX31856_ONESHOT = 0,
    MAX31856_CONTINUOUS = 1,
};

class Adafruit_MAX31856 {
public:
    Adafruit_MAX31856(int8_t cs, int8_t mosi = -1, int8_t miso = -1, int8_t sck = -1)
        : _cs(cs) { (void)mosi; (void)miso; (void)sck; }

    bool begin() { _initialized = true; return true; }

    float readThermocoupleTemperature() { return _initialized ? _tcTemp : 0.0f; }
    float readCJTemperature() { return _initialized ? _cjTemp : 0.0f; }

    void setThermocoupleType(max31856_thermocoupletype_t type) { _tcType = type; }
    max31856_thermocoupletype_t getThermocoupleType() const { return _tcType; }

    void setConversionMode(max31856_conversionmode_t mode) { _convMode = mode; }
    max31856_conversionmode_t getConversionMode() const { return _convMode; }

    void setNoiseFilter(uint8_t filter) { _noiseFilter = filter; }

    uint8_t readFault() const { return _fault; }
    void triggerOneShot() { _oneShotTriggered = true; }
    bool conversionComplete() { return true; }

    // Temperature averaging
    void setAveraging(uint8_t samples) { _averaging = samples; }

    // --- Test helpers ---
    void test_setThermocoupleTemp(float t) { _tcTemp = t; }
    void test_setCJTemp(float t) { _cjTemp = t; }
    void test_setFault(uint8_t f) { _fault = f; }
    bool test_isInitialized() const { return _initialized; }
    bool test_oneShotTriggered() const { return _oneShotTriggered; }

private:
    int8_t _cs;
    bool _initialized = false;
    float _tcTemp = 250.0f;
    float _cjTemp = 24.0f;
    max31856_thermocoupletype_t _tcType = MAX31856_TCTYPE_K;
    max31856_conversionmode_t _convMode = MAX31856_ONESHOT;
    uint8_t _noiseFilter = 0;
    uint8_t _fault = 0;
    uint8_t _averaging = 1;
    bool _oneShotTriggered = false;
};
