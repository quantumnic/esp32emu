#pragma once
// esp32emu — PCF8575 I2C 16-bit I/O Expander mock
// Provides 16 GPIO pins via I2C (address 0x20-0x27)

#include <Arduino.h>
#include <Wire.h>
#include <cstdint>

class PCF8575 {
public:
    PCF8575(uint8_t address = 0x20, TwoWire* wire = nullptr)
        : _address(address), _port(0xFFFF), _wire(wire) {}

    bool begin(uint8_t sda = 255, uint8_t scl = 255) {
        (void)sda; (void)scl;
        _initialized = true;
        return true;
    }

    bool isConnected() const { return _initialized; }

    // Write entire 16-bit port
    bool write16(uint16_t value) {
        _port = value;
        return true;
    }

    // Read entire 16-bit port
    uint16_t read16() const {
        return _port;
    }

    // Write single pin (0-15)
    bool write(uint8_t pin, uint8_t value) {
        if (pin > 15) return false;
        if (value)
            _port |= (1 << pin);
        else
            _port &= ~(1 << pin);
        return true;
    }

    // Read single pin (0-15)
    uint8_t read(uint8_t pin) const {
        if (pin > 15) return 0;
        return (_port >> pin) & 1;
    }

    // Toggle single pin
    bool toggle(uint8_t pin) {
        if (pin > 15) return false;
        _port ^= (1 << pin);
        return true;
    }

    // Set polarity inversion mask
    bool setPolarity16(uint16_t mask) { _polarity = mask; return true; }

    uint8_t getAddress() const { return _address; }
    uint8_t lastError() const { return _error; }

    // --- Test helpers ---
    void test_setInput(uint16_t value) { _port = value; }
    uint16_t test_getOutput() const { return _port; }
    void test_setPin(uint8_t pin, bool high) {
        if (pin <= 15) {
            if (high) _port |= (1 << pin);
            else _port &= ~(1 << pin);
        }
    }

private:
    uint8_t _address;
    uint16_t _port;
    uint16_t _polarity = 0x0000;
    uint8_t _error = 0;
    bool _initialized = false;
    TwoWire* _wire;
    void _suppress() { (void)_wire; }
};
