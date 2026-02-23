#pragma once
// esp32emu mock: Sensirion SEN5x (SEN50/SEN54/SEN55) — PM, VOC, NOx, T, H sensor
#include <cstdint>
#include <cstdio>

class SensirionI2CSen5x {
public:
    SensirionI2CSen5x() = default;

    void begin(void* wire = nullptr) {
        (void)wire;
        _initialized = true;
        printf("[esp32emu] SEN5x: initialized\n");
    }

    uint16_t deviceReset() { _measuring = false; return 0; }
    uint16_t startMeasurement() { _measuring = true; return 0; }
    uint16_t stopMeasurement() { _measuring = false; return 0; }

    uint16_t readMeasuredValues(
        float& pm1p0, float& pm2p5, float& pm4p0, float& pm10p0,
        float& humidity, float& temperature,
        float& vocIndex, float& noxIndex
    ) {
        if (!_measuring) return 1;
        pm1p0 = _pm1p0; pm2p5 = _pm2p5; pm4p0 = _pm4p0; pm10p0 = _pm10p0;
        humidity = _humidity; temperature = _temperature;
        vocIndex = _vocIndex; noxIndex = _noxIndex;
        return 0;
    }

    uint16_t getSerialNumber(unsigned char serial[], uint8_t maxLen) {
        const char* s = "SEN55-MOCK-001";
        uint8_t i = 0;
        while (s[i] && i < maxLen - 1) { serial[i] = s[i]; i++; }
        serial[i] = 0;
        return 0;
    }

    uint16_t setTemperatureOffsetSimple(float offset) { _tempOffset = offset; return 0; }
    uint16_t startFanCleaning() { printf("[esp32emu] SEN5x: fan cleaning\n"); return 0; }

    // ── Test helpers ──
    void _test_setValues(float pm1, float pm25, float pm4, float pm10,
                         float hum, float temp, float voc, float nox) {
        _pm1p0 = pm1; _pm2p5 = pm25; _pm4p0 = pm4; _pm10p0 = pm10;
        _humidity = hum; _temperature = temp; _vocIndex = voc; _noxIndex = nox;
    }
    bool _test_isInitialized() const { return _initialized; }
    bool _test_isMeasuring() const { return _measuring; }
    float _test_getTempOffset() const { return _tempOffset; }

private:
    bool _initialized = false;
    bool _measuring = false;
    float _pm1p0 = 5.0f, _pm2p5 = 12.0f, _pm4p0 = 15.0f, _pm10p0 = 18.0f;
    float _humidity = 45.0f, _temperature = 22.5f;
    float _vocIndex = 100.0f, _noxIndex = 1.0f;
    float _tempOffset = 0.0f;
};
