#pragma once
// esp32emu — Adafruit MAX31855 thermocouple mock

#include <cstdint>
#include <cstdio>
#include <cmath>

class Adafruit_MAX31855 {
public:
    Adafruit_MAX31855(int8_t clk, int8_t cs, int8_t miso)
        : clk_(clk), cs_(cs), miso_(miso) {}

    // SPI constructor
    Adafruit_MAX31855(int8_t cs, void* /*spi*/ = nullptr)
        : clk_(-1), cs_(cs), miso_(-1) {}

    bool begin() {
        initialized_ = true;
        fprintf(stderr, "[esp32emu] MAX31855: initialized (CS=%d)\n", cs_);
        return true;
    }

    double readCelsius() const { return thermocouple_temp_; }
    double readFahrenheit() const { return thermocouple_temp_ * 9.0 / 5.0 + 32.0; }
    double readInternal() const { return internal_temp_; }

    uint8_t readError() const { return error_; }

    // Error bits
    static constexpr uint8_t MAX31855_FAULT_OPEN   = 0x01;
    static constexpr uint8_t MAX31855_FAULT_SHORT_GND = 0x02;
    static constexpr uint8_t MAX31855_FAULT_SHORT_VCC = 0x04;

    // Test helpers
    void test_setTemperature(double thermocouple, double internal = 25.0) {
        thermocouple_temp_ = thermocouple;
        internal_temp_ = internal;
        error_ = 0;
    }

    void test_setError(uint8_t err) {
        error_ = err;
        thermocouple_temp_ = NAN;
    }

private:
    int8_t clk_, cs_, miso_;
    bool initialized_ = false;
    double thermocouple_temp_ = 25.0;
    double internal_temp_ = 25.0;
    uint8_t error_ = 0;
};
