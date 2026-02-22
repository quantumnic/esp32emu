// WS2812FX.h — LED effects library mock for esp32emu
// Built on top of Adafruit_NeoPixel with pre-built effects
#pragma once
#include "Adafruit_NeoPixel.h"
#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>
#include <functional>
#include <algorithm>

#define FX_MODE_STATIC              0
#define FX_MODE_BLINK               1
#define FX_MODE_BREATH              2
#define FX_MODE_COLOR_WIPE          3
#define FX_MODE_COLOR_WIPE_RANDOM   4
#define FX_MODE_RANDOM_COLOR        5
#define FX_MODE_RAINBOW             6
#define FX_MODE_RAINBOW_CYCLE       7
#define FX_MODE_SCAN                8
#define FX_MODE_DUAL_SCAN           9
#define FX_MODE_FADE                10
#define FX_MODE_THEATER_CHASE       11
#define FX_MODE_THEATER_CHASE_RAINBOW 12
#define FX_MODE_RUNNING_LIGHTS      13
#define FX_MODE_TWINKLE             14
#define FX_MODE_TWINKLE_RANDOM      15
#define FX_MODE_SPARKLE             16
#define FX_MODE_FLASH_SPARKLE       17
#define FX_MODE_HYPER_SPARKLE       18
#define FX_MODE_STROBE              19
#define FX_MODE_STROBE_RAINBOW      20
#define FX_MODE_MULTI_STROBE        21
#define FX_MODE_BLINK_RAINBOW       22
#define FX_MODE_CHASE_WHITE         23
#define FX_MODE_CHASE_COLOR         24
#define FX_MODE_CHASE_RANDOM        25
#define FX_MODE_CHASE_RAINBOW       26
#define FX_MODE_CHASE_FLASH         27
#define FX_MODE_CHASE_FLASH_RANDOM  28
#define FX_MODE_CHASE_RAINBOW_WHITE 29
#define FX_MODE_COLOR_SWEEP_RANDOM  30
#define FX_MODE_RUNNING_COLOR       31
#define FX_MODE_RUNNING_RED_BLUE    32
#define FX_MODE_RUNNING_RANDOM      33
#define FX_MODE_LARSON_SCANNER      34
#define FX_MODE_COMET               35
#define FX_MODE_FIREWORKS           36
#define FX_MODE_FIREWORKS_RANDOM    37
#define FX_MODE_MERRY_CHRISTMAS     38
#define FX_MODE_FIRE_FLICKER        39
#define FX_MODE_FIRE_FLICKER_SOFT   40
#define FX_MODE_FIRE_FLICKER_INTENSE 41
#define FX_MODE_CIRCUS_COMBUSTUS    42
#define FX_MODE_HALLOWEEN           43
#define FX_MODE_BICOLOR_CHASE       44
#define FX_MODE_TRICOLOR_CHASE      45
#define FX_MODE_ICU                 46
#define MODE_COUNT                  47

class WS2812FX : public Adafruit_NeoPixel {
public:
    WS2812FX(uint16_t num, uint8_t pin, uint8_t type = NEO_GRB + NEO_KHZ800)
        : Adafruit_NeoPixel(num, pin, type), _mode(FX_MODE_STATIC), _speed(1000),
          _brightness(255), _running(false), _reverse(false), _color{0xFF0000, 0x00FF00, 0x0000FF},
          _service_count(0) {}

    void init() { begin(); }

    void service() {
        if (!_running) return;
        _service_count++;
        if (_mode_callback) _mode_callback();
    }

    void start() { _running = true; printf("[esp32emu] WS2812FX: started, mode=%u\n", _mode); }
    void stop()  { _running = false; printf("[esp32emu] WS2812FX: stopped\n"); }
    bool isRunning() const { return _running; }

    void setMode(uint8_t m) {
        _mode = (m < MODE_COUNT) ? m : 0;
        printf("[esp32emu] WS2812FX: mode=%u (%s)\n", _mode, getModeName(_mode));
    }
    uint8_t getMode() const { return _mode; }

    void setSpeed(uint16_t s) { _speed = s; }
    uint16_t getSpeed() const { return _speed; }

    void setColor(uint32_t c) { _color[0] = c; }
    void setColor(uint8_t r, uint8_t g, uint8_t b) { _color[0] = ((uint32_t)r << 16) | ((uint32_t)g << 8) | b; }
    void setColors(uint8_t idx, uint32_t c) { if (idx < 3) _color[idx] = c; }
    uint32_t getColor() const { return _color[0]; }
    uint32_t getColor(uint8_t idx) const { return (idx < 3) ? _color[idx] : 0; }

    void setBrightness(uint8_t b) {
        _brightness = b;
        Adafruit_NeoPixel::setBrightness(b);
    }
    uint8_t getBrightness() const { return _brightness; }

    void setReverse(bool rev) { _reverse = rev; }
    bool getReverse() const { return _reverse; }

    uint8_t getModeCount() const { return MODE_COUNT; }

    const char* getModeName(uint8_t m) const {
        static const char* names[] = {
            "Static", "Blink", "Breath", "Color Wipe", "Color Wipe Random",
            "Random Color", "Rainbow", "Rainbow Cycle", "Scan", "Dual Scan",
            "Fade", "Theater Chase", "Theater Chase Rainbow", "Running Lights",
            "Twinkle", "Twinkle Random", "Sparkle", "Flash Sparkle", "Hyper Sparkle",
            "Strobe", "Strobe Rainbow", "Multi Strobe", "Blink Rainbow",
            "Chase White", "Chase Color", "Chase Random", "Chase Rainbow",
            "Chase Flash", "Chase Flash Random", "Chase Rainbow White",
            "Color Sweep Random", "Running Color", "Running Red Blue",
            "Running Random", "Larson Scanner", "Comet", "Fireworks",
            "Fireworks Random", "Merry Christmas", "Fire Flicker",
            "Fire Flicker Soft", "Fire Flicker Intense", "Circus Combustus",
            "Halloween", "Bicolor Chase", "Tricolor Chase", "ICU"
        };
        return (m < MODE_COUNT) ? names[m] : "Unknown";
    }

    void setCustomMode(std::function<void()> fn) { _mode_callback = fn; }

    // Test helpers
    uint32_t getServiceCount() const { return _service_count; }
    void resetServiceCount() { _service_count = 0; }

private:
    uint8_t  _mode;
    uint16_t _speed;
    uint8_t  _brightness;
    bool     _running;
    bool     _reverse;
    uint32_t _color[3];
    uint32_t _service_count;
    std::function<void()> _mode_callback;
};
