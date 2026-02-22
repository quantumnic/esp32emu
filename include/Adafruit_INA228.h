#pragma once
#include <cstdint>
#include <cstdio>

class Adafruit_INA228 {
public:
    Adafruit_INA228(uint8_t addr = 0x40) : _addr(addr) {}

    bool begin() { _started = true; return true; }

    float readBusVoltage() const { return _bus_voltage; }
    float readShuntVoltage() const { return _shunt_voltage; }
    float readCurrent() const { return _current; }
    float readPower() const { return _power; }
    float readEnergy() const { return _energy; }
    float readCharge() const { return _charge; }

    // Configuration
    void setShunt(float shunt_ohms, float max_current) {
        _shunt_ohms = shunt_ohms;
        _max_current = max_current;
    }
    void setAveragingCount(uint16_t count) { _avg_count = count; }
    void setConversionTime(uint16_t bus_us, uint16_t shunt_us) {
        _bus_conv_us = bus_us;
        _shunt_conv_us = shunt_us;
    }
    void resetEnergy() { _energy = 0.0f; }
    void resetCharge() { _charge = 0.0f; }

    bool alertFunctionFlag() const { return _alert; }
    bool conversionReady() const { return _conv_ready; }
    bool mathOverflow() const { return _overflow; }

    // Test helpers
    void _test_set_bus_voltage(float v) { _bus_voltage = v; }
    void _test_set_shunt_voltage(float v) { _shunt_voltage = v; }
    void _test_set_current(float a) { _current = a; }
    void _test_set_power(float w) { _power = w; }
    void _test_set_energy(float j) { _energy = j; }
    void _test_set_charge(float c) { _charge = c; }
    void _test_set_alert(bool a) { _alert = a; }
    void _test_set_conv_ready(bool r) { _conv_ready = r; }
    void _test_set_overflow(bool o) { _overflow = o; }

private:
    uint8_t _addr;
    bool _started = false;
    float _bus_voltage = 3.3f;
    float _shunt_voltage = 0.01f;
    float _current = 0.1f;
    float _power = 0.33f;
    float _energy = 0.0f;
    float _charge = 0.0f;
    float _shunt_ohms = 0.1f;
    float _max_current = 3.2f;
    uint16_t _avg_count = 1;
    uint16_t _bus_conv_us = 1052;
    uint16_t _shunt_conv_us = 1052;
    bool _alert = false;
    bool _conv_ready = true;
    bool _overflow = false;
};
