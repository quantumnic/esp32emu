#pragma once
// esp32emu — SparkFun BME280 environmental sensor mock

#include <cstdint>
#include <cstdio>

// Oversampling settings
#define BME280_OVERSAMP_SKIPPED 0x00
#define BME280_OVERSAMP_1X      0x01
#define BME280_OVERSAMP_2X      0x02
#define BME280_OVERSAMP_4X      0x03
#define BME280_OVERSAMP_8X      0x04
#define BME280_OVERSAMP_16X     0x05

// Standby times
#define BME280_STANDBY_0_5_MS   0x00
#define BME280_STANDBY_62_5_MS  0x01
#define BME280_STANDBY_125_MS   0x02
#define BME280_STANDBY_250_MS   0x03
#define BME280_STANDBY_500_MS   0x04
#define BME280_STANDBY_1000_MS  0x05

// Filter settings
#define BME280_FILTER_OFF  0x00
#define BME280_FILTER_2    0x01
#define BME280_FILTER_4    0x02
#define BME280_FILTER_8    0x03
#define BME280_FILTER_16   0x04

// Modes
#define BME280_MODE_SLEEP   0x00
#define BME280_MODE_FORCED  0x01
#define BME280_MODE_NORMAL  0x03

// I2C addresses
#define BME280_I2C_ADDR_PRIMARY   0x77
#define BME280_I2C_ADDR_SECONDARY 0x76

class BME280 {
public:
    BME280() = default;

    // Settings struct
    struct SensorSettings {
        uint8_t commInterface = 0; // 0=I2C, 1=SPI
        uint8_t I2CAddress = BME280_I2C_ADDR_PRIMARY;
        uint8_t chipSelectPin = 10;
        uint8_t runMode = BME280_MODE_NORMAL;
        uint8_t tempOverSample = BME280_OVERSAMP_1X;
        uint8_t pressOverSample = BME280_OVERSAMP_1X;
        uint8_t humidOverSample = BME280_OVERSAMP_1X;
        uint8_t filter = BME280_FILTER_OFF;
        uint8_t tStandby = BME280_STANDBY_0_5_MS;
    } settings;

    bool beginI2C(uint8_t addr = BME280_I2C_ADDR_PRIMARY) {
        settings.I2CAddress = addr;
        initialized_ = true;
        fprintf(stderr, "[esp32emu] SparkFun BME280: initialized I2C at 0x%02X\n", addr);
        return true;
    }

    bool beginSPI(uint8_t csPin) {
        settings.chipSelectPin = csPin;
        settings.commInterface = 1;
        initialized_ = true;
        fprintf(stderr, "[esp32emu] SparkFun BME280: initialized SPI CS=%u\n", csPin);
        return true;
    }

    uint8_t begin() {
        initialized_ = true;
        fprintf(stderr, "[esp32emu] SparkFun BME280: initialized at 0x%02X\n", settings.I2CAddress);
        return 0x60; // chip ID for BME280
    }

    bool isMeasuring() { return false; }
    void reset() { fprintf(stderr, "[esp32emu] SparkFun BME280: reset\n"); }

    void setMode(uint8_t mode) { settings.runMode = mode; }
    uint8_t getMode() { return settings.runMode; }

    void setFilter(uint8_t f) { settings.filter = f; }
    void setStandbyTime(uint8_t t) { settings.tStandby = t; }
    void setTempOverSample(uint8_t o) { settings.tempOverSample = o; }
    void setPressureOverSample(uint8_t o) { settings.pressOverSample = o; }
    void setHumidityOverSample(uint8_t o) { settings.humidOverSample = o; }

    float readTempC() { return temperature_c_; }
    float readTempF() { return temperature_c_ * 9.0f / 5.0f + 32.0f; }
    float readFloatPressure() { return pressure_pa_; }
    float readFloatAltitudeMeters() {
        return 44330.0f * (1.0f - powf(pressure_pa_ / 101325.0f, 0.1903f));
    }
    float readFloatAltitudeFeet() {
        return readFloatAltitudeMeters() * 3.28084f;
    }
    float readFloatHumidity() { return humidity_; }

    // Calibration reference
    void setReferencePressure(float refPressure) { ref_pressure_ = refPressure; }
    float getReferencePressure() { return ref_pressure_; }

    // ── Test helpers ──
    void esp32emu_set_temperature(float t) { temperature_c_ = t; }
    void esp32emu_set_pressure(float p) { pressure_pa_ = p; }
    void esp32emu_set_humidity(float h) { humidity_ = h; }

private:
    bool initialized_ = false;
    float temperature_c_ = 22.5f;
    float pressure_pa_ = 101325.0f;
    float humidity_ = 45.0f;
    float ref_pressure_ = 101325.0f;
};

// Alias for the common include name
using SparkFunBME280 = BME280;
