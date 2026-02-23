#pragma once
// esp32emu — PCA9685 16-channel PWM servo driver mock
// Simulates the NXP PCA9685 I2C PWM controller (used by Adafruit PWM Servo Driver breakout).

#include <cstdint>
#include <cstdio>
#include <cstring>

class PCA9685 {
public:
    explicit PCA9685(uint8_t addr = 0x40) : addr_(addr) {
        memset(pwm_on_, 0, sizeof(pwm_on_));
        memset(pwm_off_, 0, sizeof(pwm_off_));
    }

    bool begin() {
        initialized_ = true;
        fprintf(stderr, "[esp32emu] PCA9685 initialized at 0x%02X\n", addr_);
        return true;
    }

    void reset() {
        memset(pwm_on_, 0, sizeof(pwm_on_));
        memset(pwm_off_, 0, sizeof(pwm_off_));
        sleep_ = false;
    }

    void setPWMFreq(float freq) {
        freq_ = freq;
        fprintf(stderr, "[esp32emu] PCA9685: PWM freq = %.1f Hz\n", freq);
    }

    float getPWMFreq() const { return freq_; }

    void setPWM(uint8_t ch, uint16_t on, uint16_t off) {
        if (ch < 16) { pwm_on_[ch] = on; pwm_off_[ch] = off; }
    }

    void setPin(uint8_t ch, uint16_t val, bool invert = false) {
        if (ch < 16) {
            uint16_t v = invert ? 4095 - val : val;
            pwm_on_[ch] = 0;
            pwm_off_[ch] = v;
        }
    }

    uint16_t getPWM(uint8_t ch, bool off = true) const {
        if (ch >= 16) return 0;
        return off ? pwm_off_[ch] : pwm_on_[ch];
    }

    void setOscillatorFrequency(uint32_t freq) { osc_freq_ = freq; }
    uint32_t getOscillatorFrequency() const { return osc_freq_; }

    void sleep() { sleep_ = true; }
    void wakeup() { sleep_ = false; }

    void writeMicroseconds(uint8_t ch, uint16_t us) {
        if (ch < 16 && freq_ > 0) {
            float pulse_len = 1000000.0f / freq_ / 4096.0f;
            uint16_t ticks = (uint16_t)(us / pulse_len);
            setPWM(ch, 0, ticks);
        }
    }

    // ── Test helpers ──
    bool isInitialized() const { return initialized_; }
    bool isSleeping() const { return sleep_; }
    uint16_t getOnTime(uint8_t ch) const { return ch < 16 ? pwm_on_[ch] : 0; }
    uint16_t getOffTime(uint8_t ch) const { return ch < 16 ? pwm_off_[ch] : 0; }

private:
    uint8_t addr_;
    bool initialized_ = false;
    bool sleep_ = false;
    float freq_ = 50.0f;
    uint32_t osc_freq_ = 25000000;
    uint16_t pwm_on_[16];
    uint16_t pwm_off_[16];
};
