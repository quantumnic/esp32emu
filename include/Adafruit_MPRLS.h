#pragma once
#include <cstdint>
#include <cstdio>

class Adafruit_MPRLS {
public:
    Adafruit_MPRLS(int8_t reset_pin = -1, int8_t eoc_pin = -1,
                   uint16_t psi_min = 0, uint16_t psi_max = 25)
        : _reset_pin(reset_pin), _eoc_pin(eoc_pin),
          _psi_min(psi_min), _psi_max(psi_max) {}

    bool begin(uint8_t addr = 0x18) { _addr = addr; _started = true; return true; }

    float readPressure() const { return _pressure_hpa; }

    uint8_t lastStatus() const { return _status; }

    // Test helpers
    void _test_set_pressure(float hpa) { _pressure_hpa = hpa; }
    void _test_set_status(uint8_t s) { _status = s; }

private:
    int8_t _reset_pin;
    int8_t _eoc_pin;
    uint16_t _psi_min;
    uint16_t _psi_max;
    uint8_t _addr = 0x18;
    bool _started = false;
    float _pressure_hpa = 1013.25f;
    uint8_t _status = 0x40; // powered, no errors
};
