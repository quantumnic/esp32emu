#pragma once
// esp32emu — MCP3008 8-Channel 10-bit ADC mock
// Microchip MCP3008 SPI analog-to-digital converter.

#include <cstdint>
#include <cstdio>
#include <cstring>

class MCP3008 {
public:
    MCP3008(uint8_t cs_pin = 5, uint8_t /*clk*/ = 18, uint8_t /*mosi*/ = 23, uint8_t /*miso*/ = 19)
        : cs_pin_(cs_pin) {
        memset(channels_, 0, sizeof(channels_));
    }

    bool begin() {
        initialized_ = true;
        fprintf(stderr, "[esp32emu] MCP3008 initialized (CS pin %d)\n", cs_pin_);
        return true;
    }

    // Read single-ended channel (0-7), returns 0-1023
    uint16_t readADC(uint8_t ch) const {
        if (ch > 7) return 0;
        return channels_[ch];
    }

    // Read differential: ch0-ch1 pair
    int16_t readADCDifferential(uint8_t pos, uint8_t neg) const {
        if (pos > 7 || neg > 7) return 0;
        return static_cast<int16_t>(channels_[pos]) - static_cast<int16_t>(channels_[neg]);
    }

    // Convert to voltage given reference voltage
    float toVoltage(uint16_t raw, float vref = 3.3f) const {
        return raw * vref / 1023.0f;
    }

    bool isInitialized() const { return initialized_; }
    uint8_t getCSPin() const { return cs_pin_; }

    // ── Test helpers ──
    void setADC(uint8_t ch, uint16_t val) {
        if (ch < 8) channels_[ch] = val > 1023 ? 1023 : val;
    }
    uint16_t getADC(uint8_t ch) const { return ch < 8 ? channels_[ch] : 0; }

private:
    uint8_t cs_pin_;
    bool initialized_ = false;
    uint16_t channels_[8];
};
