#pragma once
// esp32emu — Sensirion SCD30 CO2/Temperature/Humidity sensor mock

#include <cstdint>
#include <cstdio>

class SCD30 {
public:
    SCD30() = default;

    bool begin() { return begin(0x61); }
    bool begin(uint8_t addr) {
        addr_ = addr;
        initialized_ = true;
        fprintf(stderr, "[esp32emu] SCD30 initialized at 0x%02X\n", addr_);
        return true;
    }

    bool dataAvailable() const { return data_ready_; }

    float getCO2() const { return co2_; }
    float getTemperature() const { return temperature_; }
    float getHumidity() const { return humidity_; }

    bool setMeasurementInterval(uint16_t interval) {
        interval_ = interval; return true;
    }
    uint16_t getMeasurementInterval() const { return interval_; }

    bool setAltitudeCompensation(uint16_t alt) { altitude_ = alt; return true; }
    uint16_t getAltitudeCompensation() const { return altitude_; }

    bool setTemperatureOffset(float offset) { temp_offset_ = offset; return true; }
    float getTemperatureOffset() const { return temp_offset_; }

    bool setAutoSelfCalibration(bool enable) { asc_ = enable; return true; }
    bool getAutoSelfCalibration() const { return asc_; }

    bool setForcedRecalibrationFactor(uint16_t ppm) { frc_ = ppm; return true; }
    uint16_t getForcedRecalibrationFactor() const { return frc_; }

    bool beginMeasuring() { measuring_ = true; return true; }
    bool beginMeasuring(uint16_t pressure) { ambient_pressure_ = pressure; measuring_ = true; return true; }
    bool stopMeasuring() { measuring_ = false; return true; }

    void reset() { initialized_ = false; measuring_ = false; }

    bool isInitialized() const { return initialized_; }
    bool isMeasuring() const { return measuring_; }

    // ── Test helpers ──
    void setDataReady(bool r) { data_ready_ = r; }
    void setCO2(float ppm) { co2_ = ppm; }
    void setTemperature(float t) { temperature_ = t; }
    void setHumidity(float h) { humidity_ = h; }
    void setReadings(float co2, float temp, float hum) {
        co2_ = co2; temperature_ = temp; humidity_ = hum; data_ready_ = true;
    }

private:
    uint8_t addr_ = 0x61;
    bool initialized_ = false;
    bool measuring_ = false;
    bool data_ready_ = true;
    float co2_ = 415.0f;
    float temperature_ = 22.0f;
    float humidity_ = 45.0f;
    uint16_t interval_ = 2;
    uint16_t altitude_ = 0;
    float temp_offset_ = 0.0f;
    bool asc_ = true;
    uint16_t frc_ = 400;
    uint16_t ambient_pressure_ = 0;
};
