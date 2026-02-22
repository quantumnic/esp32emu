#pragma once
// esp32emu: MAX7219 LED matrix/7-segment driver mock
#include <cstdint>
#include <cstdio>
#include <cstring>

class MAX7219 {
public:
    MAX7219(uint8_t din, uint8_t cs, uint8_t clk, uint8_t numDevices = 1)
        : _din(din), _cs(cs), _clk(clk), _numDevices(numDevices), _intensity(7), _begun(false) {
        memset(_display, 0, sizeof(_display));
    }

    void begin() {
        _begun = true;
        printf("[esp32emu] MAX7219: begin (devices=%d)\n", _numDevices);
    }

    void shutdown(uint8_t device, bool status) {
        if (device < _numDevices) _shutdown[device] = status;
    }

    void setIntensity(uint8_t device, uint8_t intensity) {
        (void)device;
        _intensity = intensity & 0x0F;
    }

    void clearDisplay(uint8_t device) {
        if (device < _numDevices) {
            memset(_display[device], 0, 8);
        }
    }

    void setRow(uint8_t device, uint8_t row, uint8_t value) {
        if (device < _numDevices && row < 8) {
            _display[device][row] = value;
        }
    }

    void setColumn(uint8_t device, uint8_t col, uint8_t value) {
        if (device < _numDevices && col < 8) {
            for (uint8_t row = 0; row < 8; row++) {
                if (value & (1 << row))
                    _display[device][row] |= (1 << col);
                else
                    _display[device][row] &= ~(1 << col);
            }
        }
    }

    void setLed(uint8_t device, uint8_t row, uint8_t col, bool state) {
        if (device < _numDevices && row < 8 && col < 8) {
            if (state) _display[device][row] |= (1 << col);
            else _display[device][row] &= ~(1 << col);
        }
    }

    void setDigit(uint8_t device, uint8_t digit, uint8_t value, bool dp = false) {
        if (device < _numDevices && digit < 8) {
            static const uint8_t charTable[] = {
                0x7E,0x30,0x6D,0x79,0x33,0x5B,0x5F,0x70,0x7F,0x7B,
                0x77,0x1F,0x4E,0x3D,0x4F,0x47
            };
            uint8_t v = (value < 16) ? charTable[value] : 0;
            if (dp) v |= 0x80;
            _display[device][digit] = v;
        }
    }

    void setChar(uint8_t device, uint8_t digit, char value, bool dp = false) {
        (void)dp;
        if (device < _numDevices && digit < 8) {
            _display[device][digit] = (uint8_t)value;
        }
    }

    // Test helpers
    bool isBegun() const { return _begun; }
    uint8_t getIntensity() const { return _intensity; }
    uint8_t getNumDevices() const { return _numDevices; }
    uint8_t getRow(uint8_t device, uint8_t row) const {
        if (device < _numDevices && row < 8) return _display[device][row];
        return 0;
    }
    bool getLed(uint8_t device, uint8_t row, uint8_t col) const {
        if (device < _numDevices && row < 8 && col < 8)
            return (_display[device][row] >> col) & 1;
        return false;
    }

private:
    uint8_t _din, _cs, _clk;
    uint8_t _numDevices;
    uint8_t _intensity;
    bool _begun;
    bool _shutdown[8] = {};
    uint8_t _display[8][8] = {};
};
