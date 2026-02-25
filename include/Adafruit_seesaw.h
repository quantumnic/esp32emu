#pragma once
// esp32emu — Adafruit seesaw multi-function STEMMA QT driver mock
// Supports: GPIO, ADC, PWM, NeoPixel, encoder, keypad, soil sensor

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <functional>

// Seesaw module base addresses
#define SEESAW_STATUS_BASE   0x00
#define SEESAW_GPIO_BASE     0x01
#define SEESAW_ADC_BASE      0x09
#define SEESAW_DAC_BASE      0x08
#define SEESAW_TIMER_BASE    0x08
#define SEESAW_NEOPIXEL_BASE 0x0E
#define SEESAW_ENCODER_BASE  0x11
#define SEESAW_KEYPAD_BASE   0x10
#define SEESAW_TOUCH_BASE    0x0F

// Status functions
#define SEESAW_STATUS_HW_ID    0x01
#define SEESAW_STATUS_VERSION  0x02
#define SEESAW_STATUS_OPTIONS  0x03
#define SEESAW_STATUS_TEMP     0x04
#define SEESAW_STATUS_SWRST    0x7F

// GPIO functions
#define SEESAW_GPIO_DIRSET_BULK  0x02
#define SEESAW_GPIO_DIRCLR_BULK  0x03
#define SEESAW_GPIO_BULK         0x04
#define SEESAW_GPIO_BULK_SET     0x05
#define SEESAW_GPIO_BULK_CLR     0x06
#define SEESAW_GPIO_PULLENSET    0x0B
#define SEESAW_GPIO_PULLENCLR    0x0C

// Encoder functions
#define SEESAW_ENCODER_STATUS   0x00
#define SEESAW_ENCODER_POSITION 0x04
#define SEESAW_ENCODER_DELTA    0x05

// Keypad functions
#define SEESAW_KEYPAD_STATUS  0x00
#define SEESAW_KEYPAD_EVENT   0x01
#define SEESAW_KEYPAD_COUNT   0x04

// NeoPixel functions
#define SEESAW_NEOPIXEL_PIN       0x01
#define SEESAW_NEOPIXEL_SPEED     0x02
#define SEESAW_NEOPIXEL_BUF_LENGTH 0x03
#define SEESAW_NEOPIXEL_BUF       0x04
#define SEESAW_NEOPIXEL_SHOW      0x05

#define SEESAW_HW_ID_CODE 0x55

// Key event types
#define SEESAW_KEYPAD_EDGE_HIGH   0
#define SEESAW_KEYPAD_EDGE_LOW    1
#define SEESAW_KEYPAD_EDGE_FALLING 2
#define SEESAW_KEYPAD_EDGE_RISING  3

struct keyEventRaw {
    struct {
        uint8_t EDGE : 2;
        uint8_t NUM  : 6;
    } bit;
    uint8_t reg = 0;
};

union keyEvent {
    keyEventRaw raw;
    uint16_t reg;
};

class Adafruit_seesaw {
public:
    Adafruit_seesaw() = default;

    bool begin(uint8_t addr = 0x49, int8_t flow = -1, bool reset = true) {
        (void)flow; (void)reset;
        addr_ = addr;
        initialized_ = true;
        fprintf(stderr, "[esp32emu] seesaw: initialized at 0x%02X\n", addr);
        return true;
    }

    // Software reset
    void SWReset() {
        fprintf(stderr, "[esp32emu] seesaw: software reset\n");
    }

    uint32_t getVersion() { return version_; }
    uint8_t getHWID() { return SEESAW_HW_ID_CODE; }
    uint32_t getOptions() { return options_; }

    float getTemp() { return temperature_; }

    // GPIO
    void pinMode(uint8_t pin, uint8_t mode) {
        (void)pin; (void)mode;
    }
    void digitalWrite(uint8_t pin, uint8_t value) {
        if (pin < 64) gpio_state_[pin] = value;
    }
    bool digitalRead(uint8_t pin) {
        return (pin < 64) ? gpio_state_[pin] : false;
    }

    // ADC
    uint16_t analogRead(uint8_t pin) {
        return (pin < 8) ? adc_values_[pin] : 0;
    }

    // PWM
    void analogWrite(uint8_t pin, uint16_t value) {
        (void)pin; (void)value;
    }

    // Encoder
    int32_t getEncoderPosition(uint8_t encoder = 0) {
        (void)encoder;
        return encoder_position_;
    }
    int32_t getEncoderDelta(uint8_t encoder = 0) {
        (void)encoder;
        int32_t d = encoder_delta_;
        encoder_delta_ = 0;
        return d;
    }
    void setEncoderPosition(int32_t pos, uint8_t encoder = 0) {
        (void)encoder;
        encoder_position_ = pos;
    }
    void enableEncoderInterrupt(uint8_t encoder = 0) { (void)encoder; }
    void disableEncoderInterrupt(uint8_t encoder = 0) { (void)encoder; }

    // NeoPixel passthrough
    void setPixelColor(uint16_t n, uint32_t color) {
        if (n < 256) pixel_colors_[n] = color;
    }
    void show() { neopixel_shown_ = true; }
    void setBrightness(uint8_t b) { brightness_ = b; }

    // Keypad
    void setKeypadEvent(uint8_t key, uint8_t edge, bool enable = true) {
        (void)key; (void)edge; (void)enable;
    }
    void enableKeypadInterrupt() { keypad_int_enabled_ = true; }
    uint8_t getKeypadCount() { return keypad_count_; }
    bool readKeypad(keyEventRaw* buf, uint8_t count) {
        if (!buf || count == 0) return false;
        for (uint8_t i = 0; i < count && i < keypad_count_; i++) {
            buf[i] = keypad_events_[i];
        }
        uint8_t read = (count < keypad_count_) ? count : keypad_count_;
        keypad_count_ = 0;
        return read > 0;
    }

    // Touch (soil sensor variant)
    uint16_t touchRead(uint8_t pin) {
        return (pin < 4) ? touch_values_[pin] : 0;
    }

    // Generic read/write
    bool write(uint8_t regHigh, uint8_t regLow, const uint8_t* buf, uint8_t num) {
        (void)regHigh; (void)regLow; (void)buf; (void)num;
        return true;
    }
    bool read(uint8_t regHigh, uint8_t regLow, uint8_t* buf, uint8_t num, uint16_t delay_us = 125) {
        (void)regHigh; (void)regLow; (void)delay_us;
        if (buf && num > 0) memset(buf, 0, num);
        return true;
    }

    // ── Test helpers ──
    void esp32emu_set_temperature(float t) { temperature_ = t; }
    void esp32emu_set_encoder_position(int32_t pos) { encoder_position_ = pos; }
    void esp32emu_set_encoder_delta(int32_t d) { encoder_delta_ = d; }
    void esp32emu_set_adc(uint8_t pin, uint16_t val) { if (pin < 8) adc_values_[pin] = val; }
    void esp32emu_set_gpio(uint8_t pin, bool val) { if (pin < 64) gpio_state_[pin] = val; }
    void esp32emu_set_touch(uint8_t pin, uint16_t val) { if (pin < 4) touch_values_[pin] = val; }
    void esp32emu_set_version(uint32_t v) { version_ = v; }
    void esp32emu_set_options(uint32_t o) { options_ = o; }
    void esp32emu_add_keypad_event(uint8_t key, uint8_t edge) {
        if (keypad_count_ < 16) {
            keyEventRaw ev;
            ev.bit.NUM = key;
            ev.bit.EDGE = edge;
            keypad_events_[keypad_count_] = ev;
            keypad_count_++;
        }
    }
    bool esp32emu_neopixel_shown() const { return neopixel_shown_; }
    uint32_t esp32emu_get_pixel_color(uint16_t n) const { return (n < 256) ? pixel_colors_[n] : 0; }
    uint8_t esp32emu_get_brightness() const { return brightness_; }

private:
    uint8_t addr_ = 0x49;
    bool initialized_ = false;
    float temperature_ = 25.0f;
    uint32_t version_ = 0x10490000; // seesaw 1.0
    uint32_t options_ = 0xFFFFFFFF;

    // GPIO state
    bool gpio_state_[64] = {};

    // ADC
    uint16_t adc_values_[8] = {};

    // Encoder
    int32_t encoder_position_ = 0;
    int32_t encoder_delta_ = 0;

    // NeoPixel
    uint32_t pixel_colors_[256] = {};
    bool neopixel_shown_ = false;
    uint8_t brightness_ = 255;

    // Keypad
    bool keypad_int_enabled_ = false;
    uint8_t keypad_count_ = 0;
    keyEventRaw keypad_events_[16] = {};

    // Touch
    uint16_t touch_values_[4] = {500, 500, 500, 500};
};

// Convenience class for rotary encoder + NeoPixel (I2C QT Rotary Encoder)
class Adafruit_seesaw_RotaryEncoder : public Adafruit_seesaw {
public:
    Adafruit_seesaw_RotaryEncoder() = default;
};
