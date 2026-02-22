#pragma once
// esp32emu — Adafruit AMG88xx (Grid-EYE) thermal camera mock
// 8x8 IR thermal sensor array with I2C interface.

#include "Wire.h"
#include <cstdint>
#include <cstring>
#include <cstdio>

#define AMG88xx_PIXEL_ARRAY_SIZE 64
#define AMG88xx_ADDRESS 0x69

// Power modes
enum amg88xx_power_mode {
    AMG88xx_NORMAL_MODE = 0x00,
    AMG88xx_SLEEP_MODE  = 0x10,
    AMG88xx_STANDBY_60  = 0x20,
    AMG88xx_STANDBY_10  = 0x21
};

// Frame rates
enum amg88xx_frame_rate {
    AMG88xx_FPS_10 = 0x00,
    AMG88xx_FPS_1  = 0x01
};

class Adafruit_AMG88xx {
public:
    Adafruit_AMG88xx() {
        memset(_pixels, 0, sizeof(_pixels));
        _thermistor = 25.0f;
    }

    bool begin(uint8_t addr = AMG88xx_ADDRESS, TwoWire* wire = nullptr) {
        (void)wire;
        _addr = addr;
        _initialized = true;
        fprintf(stderr, "[esp32emu] AMG88xx: init addr=0x%02X\n", addr);
        return true;
    }

    // Read 8x8 pixel array (temperatures in °C)
    void readPixels(float* buf, uint8_t size = AMG88xx_PIXEL_ARRAY_SIZE) {
        uint8_t count = (size < AMG88xx_PIXEL_ARRAY_SIZE) ? size : AMG88xx_PIXEL_ARRAY_SIZE;
        memcpy(buf, _pixels, count * sizeof(float));
    }

    // Read internal thermistor
    float readThermistor() const { return _thermistor; }

    // Set power mode
    void setMovingAverageMode(bool enabled) { _avg_mode = enabled; }
    bool getMovingAverageMode() const { return _avg_mode; }

    void setPowerMode(amg88xx_power_mode mode) { _power_mode = mode; }
    amg88xx_power_mode getPowerMode() const { return _power_mode; }

    void setFrameRate(amg88xx_frame_rate rate) { _frame_rate = rate; }
    amg88xx_frame_rate getFrameRate() const { return _frame_rate; }

    // Interrupt support
    void enableInterrupt() { _interrupt_enabled = true; }
    void disableInterrupt() { _interrupt_enabled = false; }
    bool isInterruptEnabled() const { return _interrupt_enabled; }

    void setInterruptLevels(float high, float low, float hysteresis = 0) {
        _int_high = high; _int_low = low; _int_hyst = hysteresis;
    }
    float getInterruptHighLevel() const { return _int_high; }
    float getInterruptLowLevel() const { return _int_low; }

    // Test helpers
    void _injectPixels(const float* data, uint8_t count = AMG88xx_PIXEL_ARRAY_SIZE) {
        uint8_t n = (count < AMG88xx_PIXEL_ARRAY_SIZE) ? count : AMG88xx_PIXEL_ARRAY_SIZE;
        memcpy(_pixels, data, n * sizeof(float));
    }
    void _injectPixel(uint8_t index, float temp) {
        if (index < AMG88xx_PIXEL_ARRAY_SIZE) _pixels[index] = temp;
    }
    void _injectThermistor(float temp) { _thermistor = temp; }

private:
    bool _initialized = false;
    uint8_t _addr = AMG88xx_ADDRESS;
    float _pixels[AMG88xx_PIXEL_ARRAY_SIZE];
    float _thermistor = 25.0f;
    bool _avg_mode = false;
    amg88xx_power_mode _power_mode = AMG88xx_NORMAL_MODE;
    amg88xx_frame_rate _frame_rate = AMG88xx_FPS_10;
    bool _interrupt_enabled = false;
    float _int_high = 0, _int_low = 0, _int_hyst = 0;
};
