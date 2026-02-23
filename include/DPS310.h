#pragma once
// esp32emu mock: Infineon DPS310 — high-precision barometric pressure & temperature sensor
#include <cstdint>
#include <cstdio>
#include <cmath>

class Dps310 {
public:
    enum Oversampling { OSR_1 = 0, OSR_2, OSR_4, OSR_8, OSR_16, OSR_32, OSR_64, OSR_128 };
    enum Rate { RATE_1 = 0, RATE_2, RATE_4, RATE_8, RATE_16, RATE_32, RATE_64, RATE_128 };

    Dps310() = default;

    int16_t begin(void* wire = nullptr, uint8_t addr = 0x77) {
        (void)wire; (void)addr;
        _initialized = true;
        printf("[esp32emu] DPS310: initialized\n");
        return 0;
    }

    int16_t measureTempOnce(float& result, uint8_t oversampling = 7) {
        (void)oversampling;
        result = _temperature;
        return 0;
    }

    int16_t measurePressureOnce(float& result, uint8_t oversampling = 7) {
        (void)oversampling;
        result = _pressure;
        return 0;
    }

    int16_t startMeasureTempOnce(uint8_t oversampling = 7) { (void)oversampling; return 0; }
    int16_t startMeasurePressureOnce(uint8_t oversampling = 7) { (void)oversampling; return 0; }

    int16_t getSingleResult(float& result) {
        result = _lastResultIsTemp ? _temperature : _pressure;
        _lastResultIsTemp = !_lastResultIsTemp;
        return 0;
    }

    int16_t setInterruptPolarity(uint8_t pol) { (void)pol; return 0; }
    int16_t setInterruptSources(bool temp, bool prs) { (void)temp; (void)prs; return 0; }

    int16_t standby() { _standby = true; return 0; }

    float getAltitude(float seaLevelPa = 101325.0f) const {
        return 44330.0f * (1.0f - powf(_pressure / (seaLevelPa / 100.0f), 0.1903f));
    }

    // ── Test helpers ──
    void _test_setTemperature(float t) { _temperature = t; }
    void _test_setPressure(float p) { _pressure = p; }
    bool _test_isInitialized() const { return _initialized; }
    bool _test_isStandby() const { return _standby; }

private:
    bool _initialized = false;
    bool _standby = false;
    bool _lastResultIsTemp = true;
    float _temperature = 22.0f;
    float _pressure = 1013.25f;
};
