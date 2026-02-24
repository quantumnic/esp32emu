#pragma once
// esp32emu — Adafruit MPL3115A2 Altimeter/Barometer/Temperature mock
// NXP MPL3115A2 I2C sensor: pressure, altitude, temperature.

#include <cstdint>
#include <cstdio>
#include <cmath>

class Adafruit_MPL3115A2 {
public:
    Adafruit_MPL3115A2() = default;

    bool begin(uint8_t addr = 0x60) {
        addr_ = addr;
        initialized_ = true;
        fprintf(stderr, "[esp32emu] MPL3115A2 initialized at 0x%02X\n", addr_);
        return true;
    }

    float getPressure() const { return pressure_pa_; }
    float getAltitude() const { return altitude_m_; }
    float getTemperature() const { return temp_c_; }

    // Oversample ratio 1..128 (powers of 2)
    void setOversampleRate(uint8_t rate) { oversample_ = rate; }
    uint8_t getOversampleRate() const { return oversample_; }

    // Mode: barometer vs altimeter
    void setMode(uint8_t mode) { mode_ = mode; }
    uint8_t getMode() const { return mode_; }

    // Sea-level pressure for altitude calculation (Pa)
    void setSeaPressure(float pa) { sea_pressure_ = pa; }
    float getSeaPressure() const { return sea_pressure_; }

    bool isInitialized() const { return initialized_; }
    uint8_t getAddress() const { return addr_; }

    // ── Test helpers ──
    void setPressure(float pa) { pressure_pa_ = pa; }
    void setAltitude(float m) { altitude_m_ = m; }
    void setTemperatureC(float c) { temp_c_ = c; }

    static constexpr uint8_t MPL3115A2_BAROMETER = 0;
    static constexpr uint8_t MPL3115A2_ALTIMETER = 1;

private:
    uint8_t addr_ = 0x60;
    bool initialized_ = false;
    uint8_t oversample_ = 1;
    uint8_t mode_ = MPL3115A2_ALTIMETER;
    float sea_pressure_ = 101325.0f;
    float pressure_pa_ = 101325.0f;
    float altitude_m_ = 0.0f;
    float temp_c_ = 25.0f;
};
