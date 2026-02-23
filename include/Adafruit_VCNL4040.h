#pragma once
#include <cstdint>
#include <Wire.h>

// VCNL4040 proximity + ambient light sensor mock
class Adafruit_VCNL4040 {
public:
    Adafruit_VCNL4040() = default;

    bool begin(uint8_t addr = 0x60, TwoWire *wire = &Wire) {
        (void)addr; (void)wire;
        initialized_ = true;
        return true;
    }

    // Proximity
    uint16_t getProximity() { return proximity_; }
    void enableProximity(bool en) { prox_enabled_ = en; }

    enum ProxIntegTime { VCNL4040_PS_IT_1T = 0, VCNL4040_PS_IT_1_5T, VCNL4040_PS_IT_2T,
                         VCNL4040_PS_IT_2_5T, VCNL4040_PS_IT_3T, VCNL4040_PS_IT_3_5T,
                         VCNL4040_PS_IT_4T, VCNL4040_PS_IT_8T };
    void setProximityIntegrationTime(ProxIntegTime t) { prox_it_ = t; }
    ProxIntegTime getProximityIntegrationTime() { return prox_it_; }

    void setProximityHighResolution(bool en) { prox_hd_ = en; }
    bool getProximityHighResolution() { return prox_hd_; }

    // Ambient light
    uint16_t getLux() { return lux_; }
    void enableAmbientLight(bool en) { als_enabled_ = en; }

    enum ALSIntegTime { VCNL4040_ALS_IT_80MS = 0, VCNL4040_ALS_IT_160MS,
                        VCNL4040_ALS_IT_320MS, VCNL4040_ALS_IT_640MS };
    void setAmbientIntegrationTime(ALSIntegTime t) { als_it_ = t; }
    ALSIntegTime getAmbientIntegrationTime() { return als_it_; }

    // White channel
    uint16_t getWhiteLight() { return white_; }

    // Interrupts
    void setProximityLowThreshold(uint16_t t) { prox_low_thresh_ = t; }
    void setProximityHighThreshold(uint16_t t) { prox_high_thresh_ = t; }
    uint16_t getProximityLowThreshold() { return prox_low_thresh_; }
    uint16_t getProximityHighThreshold() { return prox_high_thresh_; }
    void enableProximityInterrupts(bool en) { prox_int_ = en; }

    // Test helpers
    void test_setProximity(uint16_t v) { proximity_ = v; }
    void test_setLux(uint16_t v) { lux_ = v; }
    void test_setWhiteLight(uint16_t v) { white_ = v; }
    bool test_isInitialized() const { return initialized_; }
    bool test_isProxEnabled() const { return prox_enabled_; }
    bool test_isALSEnabled() const { return als_enabled_; }

private:
    bool initialized_ = false;
    bool prox_enabled_ = false;
    bool als_enabled_ = false;
    bool prox_hd_ = false;
    bool prox_int_ = false;
    uint16_t proximity_ = 0;
    uint16_t lux_ = 250;
    uint16_t white_ = 200;
    ProxIntegTime prox_it_ = VCNL4040_PS_IT_1T;
    ALSIntegTime als_it_ = VCNL4040_ALS_IT_80MS;
    uint16_t prox_low_thresh_ = 0;
    uint16_t prox_high_thresh_ = 0xFFFF;
};
