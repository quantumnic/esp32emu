// esp32emu — Adafruit_MCP4725 mock: 12-bit DAC (I2C)
#pragma once
#include <cstdint>
#include <cstdio>

class Adafruit_MCP4725 {
public:
    Adafruit_MCP4725() : _begun(false), _value(0), _voltage(0.0f), _vref(3.3f) {}

    bool begin(uint8_t addr = 0x62, void* wire = nullptr) {
        (void)addr; (void)wire;
        _addr = addr;
        _begun = true;
        printf("[esp32emu] MCP4725: initialized (I2C 0x%02X)\n", addr);
        return true;
    }

    void setVoltage(uint16_t output, bool writeEEPROM = false) {
        (void)writeEEPROM;
        _value = output & 0x0FFF;
        _voltage = (_value / 4095.0f) * _vref;
        if (writeEEPROM)
            printf("[esp32emu] MCP4725: set %u (EEPROM)\n", _value);
    }

    uint16_t getValue() const { return _value; }

    // --- Test helpers ---
    void test_setVref(float vref) { _vref = vref; }
    float test_getVoltage() const { return _voltage; }
    uint16_t test_getValue() const { return _value; }
    bool test_isBegun() const { return _begun; }
    uint8_t test_getAddr() const { return _addr; }

private:
    bool _begun;
    uint8_t _addr = 0x62;
    uint16_t _value;
    float _voltage;
    float _vref;
};
