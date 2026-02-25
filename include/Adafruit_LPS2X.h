// esp32emu — Adafruit_LPS2X mock: LPS22/LPS25 barometric pressure sensor (I2C)
#pragma once
#include <cstdint>
#include <cstdio>

enum lps2x_rate_t {
    LPS22_RATE_ONE_SHOT = 0,
    LPS22_RATE_1_HZ,
    LPS22_RATE_10_HZ,
    LPS22_RATE_25_HZ,
    LPS22_RATE_50_HZ,
    LPS22_RATE_75_HZ
};

struct sensors_event_t_lps2x {
    float pressure;
    float temperature;
};

class Adafruit_LPS22 {
public:
    Adafruit_LPS22() : _begun(false), _pressure(1013.25f), _temperature(22.0f), _rate(LPS22_RATE_ONE_SHOT) {}

    bool begin_I2C(uint8_t addr = 0x5D, void* wire = nullptr) {
        (void)addr; (void)wire;
        _begun = true;
        printf("[esp32emu] LPS22: initialized (I2C 0x%02X)\n", addr);
        return true;
    }

    bool getEvent(sensors_event_t_lps2x* pressure, sensors_event_t_lps2x* temp) {
        if (!_begun) return false;
        if (pressure) pressure->pressure = _pressure;
        if (temp) temp->temperature = _temperature;
        return true;
    }

    void setDataRate(lps2x_rate_t rate) { _rate = rate; }
    lps2x_rate_t getDataRate() const { return _rate; }

    void reset() {
        printf("[esp32emu] LPS22: reset\n");
    }

    // --- Test helpers ---
    void test_setPressure(float p) { _pressure = p; }
    void test_setTemperature(float t) { _temperature = t; }
    float test_getPressure() const { return _pressure; }
    float test_getTemperature() const { return _temperature; }
    bool test_isBegun() const { return _begun; }

private:
    bool _begun;
    float _pressure;
    float _temperature;
    lps2x_rate_t _rate;
};

// Alias for LPS25 (same interface)
using Adafruit_LPS25 = Adafruit_LPS22;
