#pragma once
// esp32emu — MS5611 Barometric Pressure Sensor mock
// Simulates the TE Connectivity MS5611 high-resolution barometric pressure sensor.

#include <cstdint>
#include <cstdio>
#include <cmath>

class MS5611 {
public:
    explicit MS5611(uint8_t addr = 0x77) : addr_(addr) {}

    bool begin() {
        initialized_ = true;
        fprintf(stderr, "[esp32emu] MS5611 initialized at 0x%02X\n", addr_);
        return true;
    }

    int read(uint8_t bits = 12) {
        (void)bits;
        last_read_ = 0; // success
        return last_read_;
    }

    float getTemperature() const { return temperature_; }
    float getPressure() const { return pressure_; }

    // Altitude from pressure using barometric formula
    float getAltitude(float seaLevelPa = 101325.0f) const {
        return 44330.0f * (1.0f - powf(pressure_ * 100.0f / seaLevelPa, 0.1903f));
    }

    // Oversampling ratio
    void setOversampling(uint8_t osr) { osr_ = osr; }
    uint8_t getOversampling() const { return osr_; }

    // Temperature compensation
    void setTemperatureCompensation(bool on) { temp_comp_ = on; }
    bool getTemperatureCompensation() const { return temp_comp_; }

    int getLastResult() const { return last_read_; }
    uint8_t getAddress() const { return addr_; }
    bool isConnected() const { return initialized_; }

    // ── Test helpers ──
    void setTemperature(float t) { temperature_ = t; }
    void setPressure(float p) { pressure_ = p; }
    void setReadings(float tempC, float pressMbar) {
        temperature_ = tempC; pressure_ = pressMbar;
    }

private:
    uint8_t addr_;
    bool initialized_ = false;
    bool temp_comp_ = true;
    uint8_t osr_ = 12; // OSR_4096
    int last_read_ = 0;
    float temperature_ = 20.0f;  // °C
    float pressure_ = 1013.25f;  // mbar
};
