#pragma once
// esp32emu mock: Adafruit NAU7802 — 24-bit ADC for load cells / scales
#include <cstdint>
#include <cstdio>

class Adafruit_NAU7802 {
public:
    Adafruit_NAU7802() = default;

    bool begin(void* wire = nullptr) {
        (void)wire;
        _initialized = true;
        printf("[esp32emu] NAU7802: initialized\n");
        return true;
    }

    bool available() const { return _initialized && _dataReady; }

    int32_t read() {
        _dataReady = false;
        return _rawValue;
    }

    bool setLDO(uint8_t voltage) { _ldo = voltage; return true; }
    bool setGain(uint8_t gain) { _gain = gain; return true; }
    bool setRate(uint8_t rate) { _rate = rate; return true; }

    bool calibrate(uint8_t mode) { (void)mode; _calibrated = true; return true; }
    bool waitForCalibration(uint32_t timeout_ms = 1000) { (void)timeout_ms; return _calibrated; }

    bool enable(bool en) { _enabled = en; return true; }
    bool reset() {
        _rawValue = 0; _calibrated = false; _enabled = true;
        return true;
    }

    // ── Test helpers ──
    void _test_setValue(int32_t value) { _rawValue = value; _dataReady = true; }
    void _test_setDataReady(bool ready) { _dataReady = ready; }
    bool _test_isInitialized() const { return _initialized; }
    uint8_t _test_getGain() const { return _gain; }
    uint8_t _test_getRate() const { return _rate; }
    uint8_t _test_getLDO() const { return _ldo; }
    bool _test_isCalibrated() const { return _calibrated; }

private:
    bool _initialized = false;
    bool _dataReady = false;
    bool _calibrated = false;
    bool _enabled = true;
    int32_t _rawValue = 0;
    uint8_t _ldo = 0;
    uint8_t _gain = 1;
    uint8_t _rate = 10;
};
