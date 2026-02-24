#pragma once
#include <cstdint>
#include <cstdio>
#include <cmath>

class MAX44009 {
public:
    explicit MAX44009(uint8_t addr = 0x4A) : _addr(addr) {}

    bool begin() { _init = true; printf("[esp32emu] MAX44009: init at 0x%02X\n", _addr); return true; }
    bool isInitialized() const { return _init; }
    uint8_t getAddress() const { return _addr; }

    // Reading
    float getLux() const { return _lux; }
    int getError() const { return _error; }

    // Configuration
    void setManualMode(bool m) { _manual = m; }
    bool getManualMode() const { return _manual; }
    void setContinuousMode(bool c) { _continuous = c; }
    bool getContinuousMode() const { return _continuous; }
    void setIntegrationTime(uint8_t t) { _intTime = t; }
    uint8_t getIntegrationTime() const { return _intTime; }

    // Interrupt/threshold
    void setHighThreshold(float lux) { _highThresh = lux; }
    float getHighThreshold() const { return _highThresh; }
    void setLowThreshold(float lux) { _lowThresh = lux; }
    float getLowThreshold() const { return _lowThresh; }
    void setThresholdTimer(uint8_t t) { _threshTimer = t; }
    uint8_t getThresholdTimer() const { return _threshTimer; }
    bool getInterruptStatus() const {
        return (_lux > _highThresh) || (_lux < _lowThresh);
    }
    void enableInterrupt(bool en) { _intEnable = en; }
    bool isInterruptEnabled() const { return _intEnable; }

    // Test helpers
    void setLux(float lux) { _lux = lux; }
    void setError(int err) { _error = err; }

    // Constants
    static constexpr uint8_t INT_TIME_800MS = 0;
    static constexpr uint8_t INT_TIME_400MS = 1;
    static constexpr uint8_t INT_TIME_200MS = 2;
    static constexpr uint8_t INT_TIME_100MS = 3;
    static constexpr uint8_t INT_TIME_50MS  = 4;
    static constexpr uint8_t INT_TIME_25MS  = 5;
    static constexpr uint8_t INT_TIME_12MS  = 6;
    static constexpr uint8_t INT_TIME_6MS   = 7;

private:
    uint8_t _addr;
    bool _init = false;
    bool _manual = false;
    bool _continuous = true;
    bool _intEnable = false;
    uint8_t _intTime = 0;
    float _lux = 0.0f;
    int _error = 0;
    float _highThresh = 188006.0f; // MAX44009 max
    float _lowThresh = 0.0f;
    uint8_t _threshTimer = 0;
};
