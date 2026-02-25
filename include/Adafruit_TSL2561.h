#pragma once
#include <cstdint>
#include <cstdio>
#include <cmath>

// Adafruit TSL2561 luminosity/lux sensor mock
// I2C digital light sensor, wide dynamic range (0.1 – 40,000 lux)

#define TSL2561_ADDR_LOW   0x29
#define TSL2561_ADDR_FLOAT 0x39
#define TSL2561_ADDR_HIGH  0x49

#define TSL2561_INTEGRATIONTIME_13MS  0x00
#define TSL2561_INTEGRATIONTIME_101MS 0x01
#define TSL2561_INTEGRATIONTIME_402MS 0x02

#define TSL2561_GAIN_1X  0x00
#define TSL2561_GAIN_16X 0x10

class Adafruit_TSL2561_Unified {
public:
    Adafruit_TSL2561_Unified(uint8_t addr = TSL2561_ADDR_FLOAT, int32_t id = 2561)
        : addr_(addr), sensor_id_(id) {}

    bool begin() {
        initialized_ = true;
        fprintf(stderr, "[esp32emu] TSL2561: initialized at 0x%02X\n", addr_);
        return true;
    }

    void enableAutoRange(bool enable) { auto_range_ = enable; }

    void setIntegrationTime(uint8_t time) { integration_time_ = time; }
    void setGain(uint8_t gain) { gain_ = gain; }

    uint8_t getIntegrationTime() const { return integration_time_; }
    uint8_t getGain() const { return gain_; }

    // Returns luminosity for both channels
    void getLuminosity(uint16_t* broadband, uint16_t* infrared) {
        if (broadband) *broadband = broadband_;
        if (infrared) *infrared = infrared_;
    }

    // Calculate lux from raw values
    uint32_t calculateLux(uint16_t broadband, uint16_t ir) {
        if (broadband == 0) return 0;
        float ratio = (float)ir / (float)broadband;
        float lux;
        // Simplified approximation of TSL2561 lux formula
        if (ratio <= 0.50f)
            lux = 0.0304f * broadband - 0.062f * broadband * powf(ratio, 1.4f);
        else if (ratio <= 0.61f)
            lux = 0.0224f * broadband - 0.031f * ir;
        else if (ratio <= 0.80f)
            lux = 0.0128f * broadband - 0.0153f * ir;
        else if (ratio <= 1.30f)
            lux = 0.00146f * broadband - 0.00112f * ir;
        else
            lux = 0;
        return (uint32_t)(lux > 0 ? lux : 0);
    }

    // Unified sensor event
    struct sensor_event_t {
        float light; // lux
    };

    bool getEvent(sensor_event_t* event) {
        if (!event || !initialized_) return false;
        event->light = (float)calculateLux(broadband_, infrared_);
        return true;
    }

    void enable() { enabled_ = true; }
    void disable() { enabled_ = false; }

    // Test injection
    void setLuminosity(uint16_t broadband, uint16_t infrared) {
        broadband_ = broadband;
        infrared_ = infrared;
    }
    void setTestLux(float lux) {
        // Set broadband/ir to approximate the given lux (broadband-dominant)
        broadband_ = (uint16_t)(lux / 0.0304f);
        infrared_ = (uint16_t)(broadband_ * 0.2f);
    }

    bool isInitialized() const { return initialized_; }
    bool isAutoRange() const { return auto_range_; }
    bool isEnabled() const { return enabled_; }
    uint8_t getAddress() const { return addr_; }
    int32_t getSensorId() const { return sensor_id_; }

private:
    uint8_t addr_;
    int32_t sensor_id_;
    bool initialized_ = false;
    bool auto_range_ = false;
    bool enabled_ = false;
    uint8_t integration_time_ = TSL2561_INTEGRATIONTIME_13MS;
    uint8_t gain_ = TSL2561_GAIN_1X;
    uint16_t broadband_ = 500;
    uint16_t infrared_ = 100;
};
