#pragma once
// esp32emu — Adafruit NeoPixel library mock
// Renders pixel colors in terminal using true-color ANSI escape codes

#include <cstdint>
#include <cstdio>
#include <vector>
#include "esp32emu_terminal.h"

#define NEO_GRB   0x01
#define NEO_RGB   0x02
#define NEO_RGBW  0x03
#define NEO_KHZ800 0x100
#define NEO_KHZ400 0x200

class Adafruit_NeoPixel {
public:
    Adafruit_NeoPixel() = default;

    Adafruit_NeoPixel(uint16_t n, int16_t pin, uint8_t type = NEO_GRB + NEO_KHZ800)
        : num_(n), pin_(pin), type_(type), pixels_(n * 3, 0), brightness_(255) {}

    void begin() {
        fprintf(stderr, "[esp32emu] NeoPixel: %d LEDs on pin %d\n", num_, pin_);
    }

    void show() {
        fprintf(stderr, "🌈 NeoPixel [");
        for (int i = 0; i < num_; i++) {
            uint8_t r = (pixels_[i*3+0] * brightness_) / 255;
            uint8_t g = (pixels_[i*3+1] * brightness_) / 255;
            uint8_t b = (pixels_[i*3+2] * brightness_) / 255;
            esp32emu::printRGB(r, g, b, i);
        }
        fprintf(stderr, "]\n");
    }

    void setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b) {
        if (n >= num_) return;
        pixels_[n*3+0] = r;
        pixels_[n*3+1] = g;
        pixels_[n*3+2] = b;
    }

    void setPixelColor(uint16_t n, uint32_t c) {
        setPixelColor(n, (uint8_t)(c >> 16), (uint8_t)(c >> 8), (uint8_t)c);
    }

    uint32_t getPixelColor(uint16_t n) const {
        if (n >= num_) return 0;
        return ((uint32_t)pixels_[n*3+0] << 16) |
               ((uint32_t)pixels_[n*3+1] << 8) |
               (uint32_t)pixels_[n*3+2];
    }

    static uint32_t Color(uint8_t r, uint8_t g, uint8_t b) {
        return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
    }

    static uint32_t ColorHSV(uint16_t hue, uint8_t sat = 255, uint8_t val = 255) {
        // Simplified HSV -> RGB
        uint8_t region = hue / 10923;  // 65536/6
        uint16_t remainder = (hue - region * 10923) * 6;
        uint8_t p = (val * (255 - sat)) >> 8;
        uint8_t q = (val * (255 - ((sat * remainder) >> 16))) >> 8;
        uint8_t t = (val * (255 - ((sat * (65535 - remainder)) >> 16))) >> 8;
        uint8_t r, g, b;
        switch (region) {
            case 0:  r=val; g=t;   b=p;   break;
            case 1:  r=q;   g=val; b=p;   break;
            case 2:  r=p;   g=val; b=t;   break;
            case 3:  r=p;   g=q;   b=val; break;
            case 4:  r=t;   g=p;   b=val; break;
            default: r=val; g=p;   b=q;   break;
        }
        return Color(r, g, b);
    }

    static uint32_t gamma32(uint32_t c) { return c; } // no-op in emulator

    void setBrightness(uint8_t b) { brightness_ = b; }
    uint8_t getBrightness() const { return brightness_; }

    void clear() { std::fill(pixels_.begin(), pixels_.end(), 0); }

    uint16_t numPixels() const { return num_; }
    int16_t getPin() const { return pin_; }

    void fill(uint32_t c = 0, uint16_t first = 0, uint16_t count = 0) {
        if (count == 0) count = num_ - first;
        for (uint16_t i = first; i < first + count && i < num_; i++)
            setPixelColor(i, c);
    }

    void updateLength(uint16_t n) {
        num_ = n;
        pixels_.resize(n * 3, 0);
    }

    void setPin(int16_t p) { pin_ = p; }

private:
    uint16_t num_ = 0;
    int16_t pin_ = -1;
    uint8_t type_ = NEO_GRB + NEO_KHZ800;
    std::vector<uint8_t> pixels_;
    uint8_t brightness_ = 255;
};
