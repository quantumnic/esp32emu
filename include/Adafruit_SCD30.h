#pragma once
// esp32emu — Adafruit SCD30 CO2/temperature/humidity sensor mock

#include <cstdint>
#include <cstdio>
#include <cmath>

class Adafruit_SCD30 {
public:
    Adafruit_SCD30() = default;

    bool begin(uint8_t addr = 0x61) {
        addr_ = addr;
        initialized_ = true;
        fprintf(stderr, "[esp32emu] SCD30: initialized at 0x%02X\n", addr);
        return true;
    }

    bool dataReady() const { return data_ready_; }

    bool read() {
        if (!initialized_ || !data_ready_) return false;
        data_ready_ = false;
        return true;
    }

    float CO2 = 415.0f;
    float temperature = 22.5f;
    float relative_humidity = 50.0f;

    void setMeasurementInterval(uint16_t interval) {
        measurement_interval_ = interval;
        fprintf(stderr, "[esp32emu] SCD30: measurement interval = %u s\n", interval);
    }
    uint16_t getMeasurementInterval() const { return measurement_interval_; }

    void setAltitudeOffset(uint16_t alt) { altitude_ = alt; }
    uint16_t getAltitudeOffset() const { return altitude_; }

    void setTemperatureOffset(float offset) { temp_offset_ = offset; }
    float getTemperatureOffset() const { return temp_offset_; }

    void setForcedRecalibrationReference(uint16_t ref) {
        frc_ref_ = ref;
        fprintf(stderr, "[esp32emu] SCD30: forced recal reference = %u ppm\n", ref);
    }
    uint16_t getForcedRecalibrationReference() const { return frc_ref_; }

    void selfCalibrationEnabled(bool en) { asc_ = en; }
    bool selfCalibrationEnabled() const { return asc_; }

    void startContinuousMeasurement(uint16_t pressure = 0) {
        running_ = true;
        ambient_pressure_ = pressure;
        fprintf(stderr, "[esp32emu] SCD30: continuous measurement started (pressure=%u mBar)\n", pressure);
    }

    void stopContinuousMeasurement() {
        running_ = false;
        fprintf(stderr, "[esp32emu] SCD30: measurement stopped\n");
    }

    void reset() {
        fprintf(stderr, "[esp32emu] SCD30: soft reset\n");
    }

    // Test helpers
    void esp32emu_set_co2(float co2) { CO2 = co2; data_ready_ = true; }
    void esp32emu_set_temperature(float t) { temperature = t; data_ready_ = true; }
    void esp32emu_set_humidity(float h) { relative_humidity = h; data_ready_ = true; }
    void esp32emu_set_data_ready(bool r) { data_ready_ = r; }
    bool esp32emu_is_initialized() const { return initialized_; }
    bool esp32emu_is_running() const { return running_; }

private:
    bool initialized_ = false;
    bool data_ready_ = true;
    bool running_ = false;
    bool asc_ = true;
    uint8_t addr_ = 0x61;
    uint16_t measurement_interval_ = 2;
    uint16_t altitude_ = 0;
    float temp_offset_ = 0.0f;
    uint16_t frc_ref_ = 400;
    uint16_t ambient_pressure_ = 0;
};
