// Adafruit_ST7735.h — ST7735 TFT display mock for esp32emu
#pragma once
#include "Adafruit_GFX.h"
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>

// ST7735 tab types (for initR)
#define INITR_GREENTAB    0x0
#define INITR_REDTAB      0x1
#define INITR_BLACKTAB    0x2
#define INITR_MINI160x80  0x4
#define INITR_144GREENTAB 0x1

// ST7735 command constants (unused in mock but defined for compatibility)
#define ST7735_TFTWIDTH_128   128
#define ST7735_TFTWIDTH_80     80
#define ST7735_TFTHEIGHT_128  128
#define ST7735_TFTHEIGHT_160  160

// Common colors
#define ST77XX_BLACK   0x0000
#define ST77XX_WHITE   0xFFFF
#define ST77XX_RED     0xF800
#define ST77XX_GREEN   0x07E0
#define ST77XX_BLUE    0x001F
#define ST77XX_CYAN    0x07FF
#define ST77XX_MAGENTA 0xF81F
#define ST77XX_YELLOW  0xFFE0
#define ST77XX_ORANGE  0xFC00

class Adafruit_ST7735 : public Adafruit_GFX {
public:
    Adafruit_ST7735(int8_t cs, int8_t dc, int8_t rst = -1)
        : Adafruit_GFX(128, 160), _cs(cs), _dc(dc), _rst(rst) {
        _framebuffer.resize(128 * 160, 0);
    }

    Adafruit_ST7735(int8_t cs, int8_t dc, int8_t mosi, int8_t sclk, int8_t rst = -1)
        : Adafruit_GFX(128, 160), _cs(cs), _dc(dc), _rst(rst) {
        (void)mosi; (void)sclk;
        _framebuffer.resize(128 * 160, 0);
    }

    // initB for ST7735B
    void initB() {
        _tabType = 0xFF;
        _width = 128; _height = 160;
        _framebuffer.resize(_width * _height, 0);
        printf("[esp32emu] ST7735: initB 128x160 (CS=%d, DC=%d)\n", _cs, _dc);
    }

    // initR for ST7735R (various tab colors)
    void initR(uint8_t options = INITR_GREENTAB) {
        _tabType = options;
        switch (options) {
            case INITR_MINI160x80:
                _width = 80; _height = 160;
                break;
            case INITR_144GREENTAB:
                _width = 128; _height = 128;
                break;
            default:
                _width = 128; _height = 160;
                break;
        }
        _framebuffer.resize(_width * _height, 0);
        printf("[esp32emu] ST7735: initR(%d) %dx%d (CS=%d, DC=%d)\n",
               options, _width, _height, _cs, _dc);
    }

    void setRotation(uint8_t r) {
        _rotation = r & 3;
        printf("[esp32emu] ST7735: rotation=%d\n", _rotation);
    }

    uint8_t getRotation() const { return _rotation; }

    void invertDisplay(bool i) {
        _invert = i;
        printf("[esp32emu] ST7735: invert=%s\n", i ? "on" : "off");
    }

    bool getInvert() const { return _invert; }

    void enableSleep(bool s) {
        _sleep = s;
        printf("[esp32emu] ST7735: sleep=%s\n", s ? "on" : "off");
    }

    bool isSleeping() const { return _sleep; }

    void enableDisplay(bool e) {
        printf("[esp32emu] ST7735: display=%s\n", e ? "on" : "off");
    }

    void setAddrWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
        _addrX = x; _addrY = y; _addrW = w; _addrH = h;
    }

    void drawPixel(int16_t x, int16_t y, uint16_t color) override {
        if (x >= 0 && x < _width && y >= 0 && y < _height) {
            _framebuffer[y * _width + x] = color;
        }
    }

    void fillScreen(uint16_t color) override {
        std::fill(_framebuffer.begin(), _framebuffer.end(), color);
    }

    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) override {
        for (int16_t j = y; j < y + h && j < _height; j++) {
            for (int16_t i = x; i < x + w && i < _width; i++) {
                if (i >= 0 && j >= 0) _framebuffer[j * _width + i] = color;
            }
        }
    }

    void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
        fillRect(x, y, w, 1, color);
    }

    void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
        fillRect(x, y, 1, h, color);
    }

    void setSPISpeed(uint32_t freq) { _spiFreq = freq; }
    uint32_t getSPISpeed() const { return _spiFreq; }

    // Color conversion helpers
    static uint16_t color565(uint8_t r, uint8_t g, uint8_t b) {
        return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
    }

    // Test helpers
    uint16_t getPixel(int16_t x, int16_t y) const {
        if (x >= 0 && x < _width && y >= 0 && y < _height)
            return _framebuffer[y * _width + x];
        return 0;
    }

    uint8_t getTabType() const { return _tabType; }
    const std::vector<uint16_t>& getFramebuffer() const { return _framebuffer; }

private:
    int8_t _cs, _dc;
    [[maybe_unused]] int8_t _rst;
    uint8_t _rotation = 0;
    uint8_t _tabType = 0;
    bool _invert = false;
    bool _sleep = false;
    uint32_t _spiFreq = 40000000;
    uint16_t _addrX = 0, _addrY = 0, _addrW = 0, _addrH = 0;
    std::vector<uint16_t> _framebuffer;
};
