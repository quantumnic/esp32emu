#pragma once
#include <cstdint>
#include <cstdio>

class Adafruit_PCT2075 {
public:
    Adafruit_PCT2075(uint8_t addr = 0x37) : _addr(addr) {}

    bool begin() { _started = true; return true; }

    float getTemperature() const { return _temperature; }

    // Overtemperature shutdown threshold
    void setHighTemperatureThreshold(float t) { _tos = t; }
    float getHighTemperatureThreshold() const { return _tos; }

    // Hysteresis threshold
    void setTemperatureHysteresis(float t) { _thyst = t; }
    float getTemperatureHysteresis() const { return _thyst; }

    // Idle time between conversions (100ms steps)
    void setIdleTime(uint8_t time) { _idle_time = time; }
    uint8_t getIdleTime() const { return _idle_time; }

    // Fault queue (1, 2, 4, 6)
    void setFaultCount(uint8_t count) { _fault_count = count; }
    uint8_t getFaultCount() const { return _fault_count; }

    // Alert mode: 0 = comparator, 1 = interrupt
    void setAlertMode(uint8_t mode) { _alert_mode = mode; }
    uint8_t getAlertMode() const { return _alert_mode; }

    // Alert polarity: 0 = active low, 1 = active high
    void setAlertPolarity(uint8_t pol) { _alert_polarity = pol; }
    uint8_t getAlertPolarity() const { return _alert_polarity; }

    // Test helpers
    void _test_set_temperature(float t) { _temperature = t; }

private:
    uint8_t _addr;
    bool _started = false;
    float _temperature = 25.0f;
    float _tos = 80.0f;
    float _thyst = 75.0f;
    uint8_t _idle_time = 1;
    uint8_t _fault_count = 1;
    uint8_t _alert_mode = 0;
    uint8_t _alert_polarity = 0;
};
