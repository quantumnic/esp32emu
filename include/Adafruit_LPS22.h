#pragma once
#include <cstdint>
#include <cstdio>

enum lps22_rate_t {
    LPS22_RATE_ONE_SHOT = 0,
    LPS22_RATE_1_HZ,
    LPS22_RATE_10_HZ,
    LPS22_RATE_25_HZ,
    LPS22_RATE_50_HZ,
    LPS22_RATE_75_HZ
};

class Adafruit_LPS22 {
public:
    Adafruit_LPS22() {}

    bool begin_I2C(uint8_t addr = 0x5C) { _addr = addr; _started = true; return true; }
    bool begin_SPI(int8_t cs) { _cs = cs; _started = true; return true; }

    float readPressure() const { return _pressure; }
    float readTemperature() const { return _temperature; }

    void setDataRate(lps22_rate_t rate) { _rate = rate; }
    lps22_rate_t getDataRate() const { return _rate; }

    bool pressureAvailable() const { return _data_ready; }
    bool temperatureAvailable() const { return _data_ready; }

    void reset() { _pressure = 1013.25f; _temperature = 25.0f; }

    // Test helpers
    void _test_set_pressure(float hpa) { _pressure = hpa; }
    void _test_set_temperature(float c) { _temperature = c; }
    void _test_set_data_ready(bool r) { _data_ready = r; }

private:
    uint8_t _addr = 0x5C;
    int8_t _cs = -1;
    bool _started = false;
    bool _data_ready = true;
    float _pressure = 1013.25f;
    float _temperature = 25.0f;
    lps22_rate_t _rate = LPS22_RATE_ONE_SHOT;
};
