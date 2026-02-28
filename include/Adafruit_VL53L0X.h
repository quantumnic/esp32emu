#pragma once
// esp32emu: Adafruit VL53L0X Time-of-Flight sensor mock (Adafruit wrapper)
#include <cstdint>
#include <cstdio>

class Adafruit_VL53L0X {
public:
    enum VL53L0X_Sense_config_t {
        VL53L0X_SENSE_DEFAULT = 0,
        VL53L0X_SENSE_LONG_RANGE = 1,
        VL53L0X_SENSE_HIGH_SPEED = 2,
        VL53L0X_SENSE_HIGH_ACCURACY = 3
    };

    Adafruit_VL53L0X() = default;

    bool begin(uint8_t i2c_addr = 0x29, bool debug = false,
               void* i2c = nullptr, VL53L0X_Sense_config_t sense = VL53L0X_SENSE_DEFAULT) {
        (void)debug; (void)i2c;
        _addr = i2c_addr;
        _sense = sense;
        _begun = true;
        printf("[esp32emu] Adafruit_VL53L0X: begin (addr=0x%02X)\n", _addr);
        return true;
    }

    // Single range measurement
    uint16_t readRange() { return _distance; }

    // Range status
    uint8_t readRangeStatus() { return _rangeStatus; }

    // Continuous mode
    void startRangeContinuous(uint16_t period_ms = 50) {
        _continuous = true;
        _period = period_ms;
    }
    void stopRangeContinuous() { _continuous = false; }
    bool isRangeComplete() { return _begun; }
    uint16_t readRangeResult() { return _distance; }

    // Ranging profile
    bool configSensor(VL53L0X_Sense_config_t sense) {
        _sense = sense;
        return true;
    }

    // Timeout
    void setTimeout(uint16_t t) { _timeout = t; }
    uint16_t getTimeout() const { return _timeout; }
    bool timeoutOccurred() { return false; }

    // I2C address change
    bool setAddress(uint8_t addr) { _addr = addr; return true; }

    // Test helpers
    void setDistance(uint16_t mm) { _distance = mm; }
    void setRangeStatus(uint8_t s) { _rangeStatus = s; }
    bool isBegun() const { return _begun; }
    bool isContinuous() const { return _continuous; }
    VL53L0X_Sense_config_t getSenseConfig() const { return _sense; }

private:
    uint8_t _addr = 0x29;
    bool _begun = false;
    uint16_t _distance = 100;
    uint8_t _rangeStatus = 0;  // 0 = valid
    bool _continuous = false;
    uint16_t _period = 50;
    uint16_t _timeout = 500;
    VL53L0X_Sense_config_t _sense = VL53L0X_SENSE_DEFAULT;
};
