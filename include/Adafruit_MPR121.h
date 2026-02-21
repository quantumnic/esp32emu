#pragma once
// esp32emu — Adafruit MPR121 Capacitive Touch Sensor mock
// Simulates a 12-channel capacitive touch sensor over I2C.

#include <cstdint>
#include <cstdio>

class Adafruit_MPR121 {
public:
    Adafruit_MPR121() = default;

    bool begin(uint8_t addr = 0x5A) {
        addr_ = addr;
        initialized_ = true;
        fprintf(stderr, "[esp32emu] MPR121 initialized at 0x%02X\n", addr_);
        return true;
    }

    // Read touch state bitmask (12 channels)
    uint16_t touched() const { return touch_state_; }

    // Read filtered data for a channel (0-11)
    uint16_t filteredData(uint8_t ch) const {
        return (ch < 12) ? filtered_[ch] : 0;
    }

    // Read baseline data for a channel (0-11)
    uint16_t baselineData(uint8_t ch) const {
        return (ch < 12) ? baseline_[ch] : 0;
    }

    // Set touch/release thresholds
    void setThresholds(uint8_t touch, uint8_t release) {
        touch_thresh_ = touch;
        release_thresh_ = release;
    }

    uint8_t getTouchThreshold() const { return touch_thresh_; }
    uint8_t getReleaseThreshold() const { return release_thresh_; }

    // Write a register (no-op in mock)
    void writeRegister(uint8_t reg, uint8_t value) {
        (void)reg; (void)value;
    }

    // Read a register (returns 0 in mock)
    uint8_t readRegister8(uint8_t reg) const {
        (void)reg;
        return 0;
    }

    uint8_t getAddress() const { return addr_; }
    bool isInitialized() const { return initialized_; }

    // ── Test helpers ──

    void setTouched(uint16_t mask) { touch_state_ = mask & 0x0FFF; }

    void setChannelTouched(uint8_t ch, bool touched) {
        if (ch < 12) {
            if (touched) touch_state_ |= (1 << ch);
            else touch_state_ &= ~(1 << ch);
        }
    }

    void setFilteredData(uint8_t ch, uint16_t val) {
        if (ch < 12) filtered_[ch] = val;
    }

    void setBaselineData(uint8_t ch, uint16_t val) {
        if (ch < 12) baseline_[ch] = val;
    }

private:
    uint8_t addr_ = 0x5A;
    bool initialized_ = false;
    uint16_t touch_state_ = 0;
    uint16_t filtered_[12] = {};
    uint16_t baseline_[12] = {200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200};
    uint8_t touch_thresh_ = 12;
    uint8_t release_thresh_ = 6;
};
