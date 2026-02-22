// Adafruit_MCP4728.h — 4-channel 12-bit DAC mock for esp32emu
#pragma once
#include <cstdint>
#include <cstdio>

typedef enum {
    MCP4728_VREF_VDD = 0,
    MCP4728_VREF_INTERNAL,
} MCP4728_vref_t;

typedef enum {
    MCP4728_PD_NORMAL = 0,
    MCP4728_PD_GND_1K,
    MCP4728_PD_GND_100K,
    MCP4728_PD_GND_500K,
} MCP4728_pd_mode_t;

typedef enum {
    MCP4728_GAIN_1X = 0,
    MCP4728_GAIN_2X,
} MCP4728_gain_t;

typedef enum {
    MCP4728_CHANNEL_A = 0,
    MCP4728_CHANNEL_B,
    MCP4728_CHANNEL_C,
    MCP4728_CHANNEL_D,
} MCP4728_channel_t;

namespace mcp4728_test {
    inline uint16_t* channels() { static uint16_t c[4] = {}; return c; }
    inline MCP4728_vref_t* vrefs() { static MCP4728_vref_t v[4] = {}; return v; }
    inline MCP4728_gain_t* gains() { static MCP4728_gain_t g[4] = {}; return g; }
    inline bool& available() { static bool v = true; return v; }
    inline void reset() {
        for (int i = 0; i < 4; i++) { channels()[i] = 0; vrefs()[i] = MCP4728_VREF_VDD; gains()[i] = MCP4728_GAIN_1X; }
        available() = true;
    }
}

class Adafruit_MCP4728 {
public:
    Adafruit_MCP4728() : _begun(false) {}

    bool begin(uint8_t addr = 0x60) {
        (void)addr;
        if (!mcp4728_test::available()) return false;
        _begun = true;
        printf("[esp32emu] MCP4728: initialized (0x%02X)\n", addr);
        return true;
    }

    bool setChannelValue(MCP4728_channel_t ch, uint16_t val,
                         MCP4728_vref_t vref = MCP4728_VREF_VDD,
                         MCP4728_gain_t gain = MCP4728_GAIN_1X,
                         MCP4728_pd_mode_t pd = MCP4728_PD_NORMAL) {
        (void)pd;
        if (!_begun || ch > MCP4728_CHANNEL_D) return false;
        if (val > 4095) val = 4095;
        mcp4728_test::channels()[ch] = val;
        mcp4728_test::vrefs()[ch] = vref;
        mcp4728_test::gains()[ch] = gain;
        return true;
    }

    bool fastWrite(uint16_t a, uint16_t b, uint16_t c, uint16_t d) {
        setChannelValue(MCP4728_CHANNEL_A, a);
        setChannelValue(MCP4728_CHANNEL_B, b);
        setChannelValue(MCP4728_CHANNEL_C, c);
        setChannelValue(MCP4728_CHANNEL_D, d);
        return true;
    }

    bool saveToEEPROM() {
        printf("[esp32emu] MCP4728: saved to EEPROM\n");
        return true;
    }

private:
    bool _begun;
};
