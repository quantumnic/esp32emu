#pragma once
// esp32emu — TMP102 Digital Temperature Sensor mock
// TI TMP102 I2C temperature sensor, ±0.5°C accuracy, 12-bit resolution.

#include <cstdint>
#include <cstdio>
#include <cmath>

class TMP102 {
public:
    TMP102(uint8_t addr = 0x48) : addr_(addr) {}

    bool begin(uint8_t /*sda*/ = 21, uint8_t /*scl*/ = 22) {
        initialized_ = true;
        fprintf(stderr, "[esp32emu] TMP102 initialized at 0x%02X\n", addr_);
        return true;
    }

    float readTempC() const { return temp_c_; }
    float readTempF() const { return temp_c_ * 9.0f / 5.0f + 32.0f; }

    void sleep() { sleeping_ = true; }
    void wakeup() { sleeping_ = false; }
    bool isSleeping() const { return sleeping_; }

    // Alert / thermostat
    void setHighTempC(float t) { high_limit_ = t; }
    void setLowTempC(float t) { low_limit_ = t; }
    float getHighTempC() const { return high_limit_; }
    float getLowTempC() const { return low_limit_; }

    bool alert() const { return temp_c_ > high_limit_ || temp_c_ < low_limit_; }

    // Extended mode: 13-bit resolution
    void setExtendedMode(bool e) { extended_ = e; }
    bool getExtendedMode() const { return extended_; }

    // Conversion rate: 0=0.25Hz, 1=1Hz, 2=4Hz (default), 3=8Hz
    void setConversionRate(uint8_t rate) { conv_rate_ = rate & 0x03; }
    uint8_t getConversionRate() const { return conv_rate_; }

    bool isInitialized() const { return initialized_; }
    uint8_t getAddress() const { return addr_; }

    // ── Test helpers ──
    void setTemperatureC(float t) { temp_c_ = t; }

private:
    uint8_t addr_;
    bool initialized_ = false;
    bool sleeping_ = false;
    bool extended_ = false;
    uint8_t conv_rate_ = 2;
    float temp_c_ = 25.0f;
    float high_limit_ = 80.0f;
    float low_limit_ = -55.0f;
};
