#pragma once
// esp32emu — Adafruit AS7341 11-channel spectral color sensor mock

#include <cstdint>
#include <cstdio>
#include <cstring>

typedef enum {
    AS7341_ATIME_29  = 29,
    AS7341_ATIME_59  = 59,
    AS7341_ATIME_100 = 100,
    AS7341_ATIME_199 = 199,
} as7341_atime_t;

typedef enum {
    AS7341_ASTEP_999  = 999,
    AS7341_ASTEP_599  = 599,
    AS7341_ASTEP_299  = 299,
} as7341_astep_t;

typedef enum {
    AS7341_GAIN_0_5X = 0,
    AS7341_GAIN_1X   = 1,
    AS7341_GAIN_2X   = 2,
    AS7341_GAIN_4X   = 3,
    AS7341_GAIN_8X   = 4,
    AS7341_GAIN_16X  = 5,
    AS7341_GAIN_32X  = 6,
    AS7341_GAIN_64X  = 7,
    AS7341_GAIN_128X = 8,
    AS7341_GAIN_256X = 9,
    AS7341_GAIN_512X = 10,
} as7341_gain_t;

typedef enum {
    AS7341_CHANNEL_415nm_F1 = 0,
    AS7341_CHANNEL_445nm_F2 = 1,
    AS7341_CHANNEL_480nm_F3 = 2,
    AS7341_CHANNEL_515nm_F4 = 3,
    AS7341_CHANNEL_555nm_F5 = 4,
    AS7341_CHANNEL_590nm_F6 = 5,
    AS7341_CHANNEL_630nm_F7 = 6,
    AS7341_CHANNEL_680nm_F8 = 7,
    AS7341_CHANNEL_CLEAR    = 8,
    AS7341_CHANNEL_NIR      = 9,
} as7341_color_channel_t;

typedef enum {
    AS7341_LED_OFF    = 0,
    AS7341_LED_12_5MA = 1,
    AS7341_LED_25MA   = 2,
    AS7341_LED_50MA   = 3,
    AS7341_LED_100MA  = 4,
} as7341_led_current_t;

class Adafruit_AS7341 {
public:
    Adafruit_AS7341() { memset(channels_, 0, sizeof(channels_)); }

    bool begin(uint8_t addr = 0x39) {
        (void)addr;
        initialized_ = true;
        fprintf(stderr, "[esp32emu] AS7341: initialized at 0x%02X\n", addr);
        return true;
    }

    void setATIME(uint16_t atime) { atime_ = atime; }
    uint16_t getATIME() const { return atime_; }

    void setASTEP(uint16_t astep) { astep_ = astep; }
    uint16_t getASTEP() const { return astep_; }

    void setGain(as7341_gain_t gain) { gain_ = gain; }
    as7341_gain_t getGain() const { return gain_; }

    bool readAllChannels() {
        if (!initialized_) return false;
        return true;
    }

    uint16_t getChannel(as7341_color_channel_t ch) const {
        if (ch < 0 || ch > 9) return 0;
        return channels_[ch];
    }

    bool readAllChannels(uint16_t* buf) {
        if (!initialized_ || !buf) return false;
        memcpy(buf, channels_, sizeof(channels_));
        return true;
    }

    // Flicker detection
    uint16_t detectFlickerHz() const { return flicker_hz_; }

    // LED control
    void enableLED(bool en) {
        led_on_ = en;
        fprintf(stderr, "[esp32emu] AS7341: LED %s\n", en ? "ON" : "OFF");
    }
    void setLEDCurrent(as7341_led_current_t current) { led_current_ = current; }

    // SMUX config
    bool enableSpectralMeasurement(bool en) {
        (void)en;
        return initialized_;
    }

    // Integration time in ms: (ATIME + 1) * (ASTEP + 1) * 2.78µs / 1000
    float getTINT() const {
        return (float)(atime_ + 1) * (float)(astep_ + 1) * 2.78f / 1000.0f;
    }

    // Test helpers
    void esp32emu_set_channel(as7341_color_channel_t ch, uint16_t val) {
        if (ch >= 0 && ch <= 9) channels_[ch] = val;
    }
    void esp32emu_set_all_channels(const uint16_t* vals) {
        memcpy(channels_, vals, sizeof(channels_));
    }
    void esp32emu_set_flicker(uint16_t hz) { flicker_hz_ = hz; }

private:
    bool initialized_ = false;
    uint16_t atime_ = 100;
    uint16_t astep_ = 999;
    as7341_gain_t gain_ = AS7341_GAIN_8X;
    uint16_t channels_[10] = {};
    uint16_t flicker_hz_ = 0;
    bool led_on_ = false;
    as7341_led_current_t led_current_ = AS7341_LED_OFF;
};
