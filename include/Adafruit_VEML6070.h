#pragma once
// esp32emu — Adafruit VEML6070 UV Light Sensor mock

#include <Arduino.h>
#include <Wire.h>
#include <cstdint>

// Integration time settings
enum veml6070_integrationtime_t {
    VEML6070_HALF_T = 0,
    VEML6070_1_T,
    VEML6070_2_T,
    VEML6070_4_T
};

class Adafruit_VEML6070 {
public:
    Adafruit_VEML6070() = default;

    void begin(veml6070_integrationtime_t itime = VEML6070_1_T) {
        _itime = itime;
        _initialized = true;
    }

    // Read raw UV intensity (16-bit)
    uint16_t readUV() const {
        return _uvRaw;
    }

    // Set integration time
    void setIntegrationTime(veml6070_integrationtime_t itime) {
        _itime = itime;
    }

    // Sleep / wake
    void sleep(bool s) { _sleeping = s; }
    bool isSleeping() const { return _sleeping; }

    // --- Test helpers ---
    void test_setUV(uint16_t raw) { _uvRaw = raw; }

private:
    bool _initialized = false;
    bool _sleeping = false;
    veml6070_integrationtime_t _itime = VEML6070_1_T;
    uint16_t _uvRaw = 0;
};
