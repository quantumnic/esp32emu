#pragma once
#include <cstdint>

// Adafruit TSL2591 high-dynamic-range lux sensor mock
// Gain: 1x, 25x, 428x, 9876x
// Integration: 100ms-600ms

#define TSL2591_GAIN_LOW   0x00
#define TSL2591_GAIN_MED   0x10
#define TSL2591_GAIN_HIGH  0x20
#define TSL2591_GAIN_MAX   0x30

#define TSL2591_INTEGRATIONTIME_100MS 0x00
#define TSL2591_INTEGRATIONTIME_200MS 0x01
#define TSL2591_INTEGRATIONTIME_300MS 0x02
#define TSL2591_INTEGRATIONTIME_400MS 0x03
#define TSL2591_INTEGRATIONTIME_500MS 0x04
#define TSL2591_INTEGRATIONTIME_600MS 0x05

class Adafruit_TSL2591 {
public:
    explicit Adafruit_TSL2591(int32_t sensorID = -1) : sensor_id_(sensorID) {}

    bool begin() { initialized_ = true; return true; }

    void setGain(uint8_t gain) { gain_ = gain; }
    uint8_t getGain() const { return gain_; }

    void setTiming(uint8_t timing) { timing_ = timing; }
    uint8_t getTiming() const { return timing_; }

    void enable() { enabled_ = true; }
    void disable() { enabled_ = false; }

    uint32_t getFullLuminosity() {
        return ((uint32_t)ir_ << 16) | visible_;
    }
    uint16_t getLuminosity(uint8_t channel) {
        if (channel == 0) return visible_;  // full spectrum
        if (channel == 1) return ir_;        // IR
        return visible_ > ir_ ? visible_ - ir_ : 0;  // visible only
    }

    float calculateLux(uint16_t ch0, uint16_t ch1) {
        if (ch0 == 0) return 0.0f;
        float ratio = (float)ch1 / (float)ch0;
        float lux = ((float)ch0 - 1.7f * (float)ch1) / (1.0f + (float)(timing_ + 1) * 100.0f / 100.0f);
        if (ratio > 0.8f) lux = 0.0f; // saturated
        (void)ratio;
        return lux > 0 ? lux : 0.0f;
    }

    // Test injection
    void setLuminosity(uint16_t visible, uint16_t ir) { visible_ = visible; ir_ = ir; }

    bool isInitialized() const { return initialized_; }
    int32_t getSensorID() const { return sensor_id_; }

private:
    int32_t sensor_id_ = -1;
    bool initialized_ = false;
    bool enabled_ = false;
    uint8_t gain_ = TSL2591_GAIN_MED;
    uint8_t timing_ = TSL2591_INTEGRATIONTIME_100MS;
    uint16_t visible_ = 0;
    uint16_t ir_ = 0;
};
