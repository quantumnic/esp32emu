#pragma once
#include <cstdint>
#include <cstdio>

class INA226 {
public:
    explicit INA226(uint8_t addr = 0x40) : _addr(addr) {}

    bool begin() { _init = true; return true; }
    bool isInitialized() const { return _init; }
    uint8_t getAddress() const { return _addr; }

    // Configuration
    void setAverage(uint8_t avg) { _avg = avg; }
    uint8_t getAverage() const { return _avg; }
    void setBusConversionTime(uint8_t ct) { _busCT = ct; }
    uint8_t getBusConversionTime() const { return _busCT; }
    void setShuntConversionTime(uint8_t ct) { _shuntCT = ct; }
    uint8_t getShuntConversionTime() const { return _shuntCT; }
    void setMode(uint8_t mode) { _mode = mode; }
    uint8_t getMode() const { return _mode; }

    // Calibration
    void calibrate(float shuntR, float maxCurrent) {
        _shuntR = shuntR;
        _maxCurrent = maxCurrent;
        _calibrated = true;
    }
    bool isCalibrated() const { return _calibrated; }

    // Readings
    float getBusVoltage() const { return _busV; }
    float getShuntVoltage() const { return _shuntmV; }
    float getCurrent() const { return _calibrated ? (_shuntmV / 1000.0f) / _shuntR : 0.0f; }
    float getPower() const { return getBusVoltage() * getCurrent(); }

    // Alert
    void setAlertLimit(float limit) { _alertLimit = limit; }
    float getAlertLimit() const { return _alertLimit; }
    void setAlertType(uint8_t type) { _alertType = type; }
    uint8_t getAlertType() const { return _alertType; }
    bool alertTriggered() const {
        if (_alertType == 1) return getCurrent() > _alertLimit;
        if (_alertType == 2) return getBusVoltage() > _alertLimit;
        return false;
    }

    // Test helpers
    void setBusVoltage(float v) { _busV = v; }
    void setShuntVoltage(float mv) { _shuntmV = mv; }

    void reset() {
        _avg = 0; _busCT = 4; _shuntCT = 4; _mode = 7;
        _busV = 0; _shuntmV = 0; _calibrated = false;
        _alertLimit = 0; _alertType = 0;
    }

    // Constants
    static constexpr uint8_t AVG_1    = 0;
    static constexpr uint8_t AVG_4    = 1;
    static constexpr uint8_t AVG_16   = 2;
    static constexpr uint8_t AVG_64   = 3;
    static constexpr uint8_t AVG_128  = 4;
    static constexpr uint8_t AVG_256  = 5;
    static constexpr uint8_t AVG_512  = 6;
    static constexpr uint8_t AVG_1024 = 7;

    static constexpr uint8_t ALERT_NONE       = 0;
    static constexpr uint8_t ALERT_OVER_CURRENT = 1;
    static constexpr uint8_t ALERT_OVER_VOLTAGE = 2;

private:
    uint8_t _addr;
    bool _init = false;
    bool _calibrated = false;
    uint8_t _avg = 0;
    uint8_t _busCT = 4;
    uint8_t _shuntCT = 4;
    uint8_t _mode = 7;
    float _shuntR = 0.1f;
    float _maxCurrent = 1.0f;
    float _busV = 0.0f;
    float _shuntmV = 0.0f;
    float _alertLimit = 0.0f;
    uint8_t _alertType = 0;
};
