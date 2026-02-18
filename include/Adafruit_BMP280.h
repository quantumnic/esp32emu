#pragma once
#include <cstdint>
#include <cstdio>
#include <cmath>

/// @brief Adafruit BMP280 pressure/temperature sensor mock for esp32emu.
/// Provides settable temperature, pressure, and altitude for testing.

class Adafruit_BMP280 {
public:
    Adafruit_BMP280() {}

    /// Initialize (I2C address ignored in mock)
    bool begin(uint8_t addr = 0x76) {
        (void)addr;
        initialized_ = true;
        return true;
    }

    float readTemperature() {
        return temperature_;
    }

    float readPressure() {
        return pressure_;
    }

    /// Approximate altitude from pressure using barometric formula
    float readAltitude(float seaLevelHpa = 1013.25f) {
        float p = pressure_ / 100.0f; // Pa → hPa
        return 44330.0f * (1.0f - powf(p / seaLevelHpa, 0.1903f));
    }

    /// Set sensor sampling (all params ignored in mock)
    void setSampling(uint8_t mode = 0, uint8_t tempSampling = 0,
                     uint8_t pressSampling = 0, uint8_t filter = 0,
                     uint8_t duration = 0) {
        (void)mode; (void)tempSampling; (void)pressSampling;
        (void)filter; (void)duration;
    }

    // ── Test helpers ──
    void setTemperature(float t) { temperature_ = t; }
    void setPressure(float p) { pressure_ = p; }
    bool isInitialized() const { return initialized_; }

private:
    bool initialized_ = false;
    float temperature_ = 25.0f;  // °C
    float pressure_ = 101325.0f; // Pa (1013.25 hPa = sea level)
};
