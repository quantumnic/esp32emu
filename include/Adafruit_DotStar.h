// Adafruit DotStar (APA102) LED strip mock for esp32emu
#pragma once
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <algorithm>

class Adafruit_DotStar {
public:
    Adafruit_DotStar(uint16_t n, uint8_t dataPin, uint8_t clockPin, uint8_t order = 0)
        : num_leds_(n), data_pin_(dataPin), clock_pin_(clockPin), order_(order) {
        pixels_ = new uint8_t[n * 3]();
    }

    Adafruit_DotStar(uint16_t n, uint8_t order = 0)
        : num_leds_(n), data_pin_(255), clock_pin_(255), order_(order), hw_spi_(true) {
        pixels_ = new uint8_t[n * 3]();
    }

    ~Adafruit_DotStar() { delete[] pixels_; }

    void begin() {
        begun_ = true;
        fprintf(stderr, "[esp32emu] DotStar: %d LEDs on pins %d/%d%s\n",
                num_leds_, data_pin_, clock_pin_, hw_spi_ ? " (HW SPI)" : "");
    }

    void show() {
        show_count_++;
        fprintf(stderr, "[esp32emu] DotStar: show() [brightness=%d]\n", brightness_);
    }

    void setPixelColor(uint16_t n, uint32_t c) {
        if (n >= num_leds_) return;
        pixels_[n * 3 + 0] = (c >> 16) & 0xFF;
        pixels_[n * 3 + 1] = (c >> 8) & 0xFF;
        pixels_[n * 3 + 2] = c & 0xFF;
    }

    void setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b) {
        setPixelColor(n, ((uint32_t)r << 16) | ((uint32_t)g << 8) | b);
    }

    uint32_t getPixelColor(uint16_t n) const {
        if (n >= num_leds_) return 0;
        return ((uint32_t)pixels_[n * 3] << 16) |
               ((uint32_t)pixels_[n * 3 + 1] << 8) |
               pixels_[n * 3 + 2];
    }

    void setBrightness(uint8_t b) { brightness_ = b; }
    uint8_t getBrightness() const { return brightness_; }

    void clear() { memset(pixels_, 0, num_leds_ * 3); }

    void fill(uint32_t color, uint16_t first = 0, uint16_t count = 0) {
        uint16_t end = (count == 0) ? num_leds_ : std::min((uint16_t)(first + count), num_leds_);
        for (uint16_t i = first; i < end; i++) setPixelColor(i, color);
    }

    uint16_t numPixels() const { return num_leds_; }

    static uint32_t Color(uint8_t r, uint8_t g, uint8_t b) {
        return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
    }

    static uint32_t ColorHSV(uint16_t hue, uint8_t sat = 255, uint8_t val = 255) {
        uint8_t r, g, b;
        uint8_t region = hue / 10923; // 65536/6
        uint16_t remainder = (hue - region * 10923) * 6;
        uint8_t p = (val * (255 - sat)) >> 8;
        uint8_t q = (val * (255 - ((sat * remainder) >> 16))) >> 8;
        uint8_t t = (val * (255 - ((sat * (65535 - remainder)) >> 16))) >> 8;
        switch (region) {
            case 0:  r = val; g = t; b = p; break;
            case 1:  r = q; g = val; b = p; break;
            case 2:  r = p; g = val; b = t; break;
            case 3:  r = p; g = q; b = val; break;
            case 4:  r = t; g = p; b = val; break;
            default: r = val; g = p; b = q; break;
        }
        return Color(r, g, b);
    }

    static uint32_t gamma32(uint32_t c) { return c; } // passthrough in mock

    // Test helpers
    uint32_t getShowCount() const { return show_count_; }
    bool isBegun() const { return begun_; }

private:
    uint16_t num_leds_;
    uint8_t data_pin_;
    uint8_t clock_pin_;
    uint8_t order_;
    bool hw_spi_ = false;
    bool begun_ = false;
    uint8_t brightness_ = 255;
    uint8_t* pixels_ = nullptr;
    uint32_t show_count_ = 0;
};

// Color order constants
#define DOTSTAR_BRG 0
#define DOTSTAR_BGR 1
#define DOTSTAR_GBR 2
#define DOTSTAR_GRB 3
#define DOTSTAR_RBG 4
#define DOTSTAR_RGB 5
