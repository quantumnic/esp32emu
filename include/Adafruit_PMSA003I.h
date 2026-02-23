#pragma once
// esp32emu — Adafruit PMSA003I (PM25AQI) particulate matter sensor mock

#include <cstdint>
#include <cstdio>

struct PM25_AQI_Data {
    // Standard (CF=1) concentrations in µg/m³
    uint16_t pm10_standard  = 5;
    uint16_t pm25_standard  = 12;
    uint16_t pm100_standard = 18;
    // Environmental concentrations in µg/m³
    uint16_t pm10_env  = 5;
    uint16_t pm25_env  = 12;
    uint16_t pm100_env = 18;
    // Particle counts per 0.1L
    uint16_t particles_03um = 1200;
    uint16_t particles_05um = 350;
    uint16_t particles_10um = 80;
    uint16_t particles_25um = 12;
    uint16_t particles_50um = 3;
    uint16_t particles_100um = 1;
};

class Adafruit_PM25AQI {
public:
    Adafruit_PM25AQI() = default;

    bool begin_I2C(uint8_t addr = 0x12) {
        (void)addr;
        initialized_ = true;
        fprintf(stderr, "[esp32emu] PMSA003I: initialized at 0x%02X\n", addr);
        return true;
    }

    bool begin_UART(void* serial) {
        (void)serial;
        initialized_ = true;
        fprintf(stderr, "[esp32emu] PMSA003I: initialized via UART\n");
        return true;
    }

    bool read(PM25_AQI_Data* data) {
        if (!initialized_ || !data) return false;
        *data = data_;
        return true;
    }

    // Test helpers
    void esp32emu_set_data(const PM25_AQI_Data& d) { data_ = d; }
    void esp32emu_set_pm25(uint16_t std_val, uint16_t env_val) {
        data_.pm25_standard = std_val;
        data_.pm25_env = env_val;
    }
    void esp32emu_set_pm10(uint16_t std_val, uint16_t env_val) {
        data_.pm10_standard = std_val;
        data_.pm10_env = env_val;
    }
    void esp32emu_set_pm100(uint16_t std_val, uint16_t env_val) {
        data_.pm100_standard = std_val;
        data_.pm100_env = env_val;
    }

private:
    bool initialized_ = false;
    PM25_AQI_Data data_;
};
