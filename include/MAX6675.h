#pragma once
// esp32emu: MAX6675 thermocouple sensor mock
#include <cstdint>
#include <cstdio>

class MAX6675 {
public:
    MAX6675(int8_t SCLK = -1, int8_t CS = -1, int8_t MISO = -1)
        : _sclk(SCLK), _cs(CS), _miso(MISO) {}

    float readCelsius() {
        printf("[esp32emu] MAX6675: read %.1f°C\n", _tempC);
        return _tempC;
    }

    float readFahrenheit() {
        return readCelsius() * 9.0f / 5.0f + 32.0f;
    }

    // esp32emu test helpers
    void setTemperature(float tempC) { _tempC = tempC; }
    float getTemperature() const { return _tempC; }
    int8_t getCS() const { return _cs; }
    int8_t getSCLK() const { return _sclk; }
    int8_t getMISO() const { return _miso; }

private:
    int8_t _sclk, _cs, _miso;
    float _tempC = 25.0f;
};
