#pragma once
#include <cstdint>
#include <cstdio>
#include "Wire.h"

// AHT10/AHT20 temperature & humidity sensor mock

#define AHTXX_ADDRESS_X38 0x38
#define AHT10_ADDRESS     0x38
#define AHT20_ADDRESS     0x38

#define AHT1x_SENSOR 10
#define AHT2x_SENSOR 20

class AHTxx {
public:
    AHTxx(uint8_t addr = AHTXX_ADDRESS_X38, uint8_t type = AHT2x_SENSOR)
        : addr_(addr), type_(type) {}

    bool begin(TwoWire* wire = nullptr) {
        (void)wire;
        initialized_ = true;
        fprintf(stderr, "[esp32emu] AHT%d: initialized at 0x%02X\n", type_, addr_);
        return true;
    }

    float readTemperature() {
        if (!initialized_) return 255.0f; // error value
        return temperature_;
    }

    float readHumidity() {
        if (!initialized_) return 255.0f;
        return humidity_;
    }

    bool softReset() { return initialized_; }

    uint8_t getStatus() const { return initialized_ ? 0x18 : 0xFF; }

    // ── Test helpers ──
    void setTemperature(float t) { temperature_ = t; }
    void setHumidity(float h) { humidity_ = h; }
    bool isInitialized() const { return initialized_; }

private:
    uint8_t addr_;
    uint8_t type_;
    bool initialized_ = false;
    float temperature_ = 23.0f;
    float humidity_ = 50.0f;
};

// Convenience aliases
using AHT10 = AHTxx;
using AHT20 = AHTxx;
