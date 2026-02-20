#pragma once
#include <cstdint>
#include <cstdio>
#include <cmath>
#include "Wire.h"

// BME280 sensor mock — temperature, humidity, pressure, altitude
// Extends BMP280 with humidity support

#define BME280_ADDRESS         0x76
#define BME280_ADDRESS_ALTERNATE 0x77

class Adafruit_BME280 {
public:
    enum sensor_sampling {
        SAMPLING_NONE  = 0,
        SAMPLING_X1    = 1,
        SAMPLING_X2    = 2,
        SAMPLING_X4    = 3,
        SAMPLING_X8    = 4,
        SAMPLING_X16   = 5
    };
    enum sensor_mode {
        MODE_SLEEP  = 0,
        MODE_FORCED = 1,
        MODE_NORMAL = 3
    };
    enum sensor_filter {
        FILTER_OFF = 0,
        FILTER_X2  = 1,
        FILTER_X4  = 2,
        FILTER_X8  = 3,
        FILTER_X16 = 4
    };
    enum standby_duration {
        STANDBY_MS_0_5  = 0,
        STANDBY_MS_10   = 6,
        STANDBY_MS_20   = 7,
        STANDBY_MS_62_5 = 1,
        STANDBY_MS_125  = 2,
        STANDBY_MS_250  = 3,
        STANDBY_MS_500  = 4,
        STANDBY_MS_1000 = 5
    };

    Adafruit_BME280() = default;

    bool begin(uint8_t addr = BME280_ADDRESS, TwoWire* wire = nullptr) {
        (void)wire;
        addr_ = addr;
        initialized_ = true;
        fprintf(stderr, "[esp32emu] BME280: initialized at 0x%02X\n", addr);
        return true;
    }

    void setSampling(sensor_mode mode = MODE_NORMAL,
                     sensor_sampling tempSampling = SAMPLING_X16,
                     sensor_sampling pressSampling = SAMPLING_X16,
                     sensor_sampling humSampling = SAMPLING_X16,
                     sensor_filter filter = FILTER_OFF,
                     standby_duration duration = STANDBY_MS_0_5) {
        (void)tempSampling; (void)pressSampling; (void)humSampling;
        (void)filter; (void)duration;
        mode_ = mode;
    }

    bool takeForcedMeasurement() {
        return initialized_;
    }

    float readTemperature() const { return temperature_; }
    float readPressure() const { return pressure_; }
    float readHumidity() const { return humidity_; }

    float readAltitude(float seaLevelPa = 101325.0f) const {
        float p = pressure_;
        if (p == 0) return 0;
        return 44330.0f * (1.0f - powf(p / seaLevelPa, 0.1903f));
    }

    float seaLevelForAltitude(float altitude, float pressure) const {
        return pressure / powf(1.0f - (altitude / 44330.0f), 5.255f);
    }

    uint32_t sensorID() const { return 0x60; } // BME280 chip ID

    // ── Test helpers ──
    void setTemperature(float t) { temperature_ = t; }
    void setPressure(float p) { pressure_ = p; }
    void setHumidity(float h) { humidity_ = h; }
    bool isInitialized() const { return initialized_; }
    uint8_t getAddress() const { return addr_; }

private:
    bool initialized_ = false;
    uint8_t addr_ = BME280_ADDRESS;
    sensor_mode mode_ = MODE_NORMAL;
    float temperature_ = 22.5f;
    float pressure_ = 101325.0f;
    float humidity_ = 45.0f;
};
