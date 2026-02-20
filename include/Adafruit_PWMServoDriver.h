#pragma once
#include <cstdint>
#include <cstdio>
#include <cstring>
#include "Wire.h"

// PCA9685 16-channel PWM servo driver mock

#define PCA9685_ADDRESS 0x40

class Adafruit_PWMServoDriver {
public:
    Adafruit_PWMServoDriver(uint8_t addr = PCA9685_ADDRESS, TwoWire& wire = Wire)
        : addr_(addr) {
        (void)wire;
        memset(pwm_on_, 0, sizeof(pwm_on_));
        memset(pwm_off_, 0, sizeof(pwm_off_));
    }

    bool begin() {
        initialized_ = true;
        fprintf(stderr, "[esp32emu] PCA9685: initialized at 0x%02X\n", addr_);
        return true;
    }

    void reset() {
        memset(pwm_on_, 0, sizeof(pwm_on_));
        memset(pwm_off_, 0, sizeof(pwm_off_));
        fprintf(stderr, "[esp32emu] PCA9685: reset\n");
    }

    void setPWMFreq(float freq) {
        freq_ = freq;
        fprintf(stderr, "[esp32emu] PCA9685: PWM frequency set to %.1f Hz\n", freq);
    }

    float getPWMFreq() const { return freq_; }

    void setPWM(uint8_t channel, uint16_t on, uint16_t off) {
        if (channel >= 16) return;
        pwm_on_[channel] = on;
        pwm_off_[channel] = off;
    }

    uint16_t getPWM(uint8_t channel, bool off = false) const {
        if (channel >= 16) return 0;
        return off ? pwm_off_[channel] : pwm_on_[channel];
    }

    void setPin(uint8_t channel, uint16_t val, bool invert = false) {
        if (channel >= 16) return;
        if (invert) val = 4095 - val;
        if (val == 4095) {
            setPWM(channel, 4096, 0);
        } else if (val == 0) {
            setPWM(channel, 0, 4096);
        } else {
            setPWM(channel, 0, val);
        }
    }

    void writeMicroseconds(uint8_t channel, uint16_t us) {
        if (channel >= 16 || freq_ <= 0) return;
        double pulseLength = 1000000.0 / freq_ / 4096.0;
        uint16_t pulse = (uint16_t)(us / pulseLength);
        setPWM(channel, 0, pulse);
    }

    void setOscillatorFrequency(uint32_t freq) { oscFreq_ = freq; }
    uint32_t getOscillatorFrequency() const { return oscFreq_; }

    void setOutputMode(bool totempole) { totempole_ = totempole; }

    void sleep() { sleeping_ = true; }
    void wakeup() { sleeping_ = false; }

    // ── Test helpers ──
    bool isInitialized() const { return initialized_; }
    uint8_t getAddress() const { return addr_; }
    bool isSleeping() const { return sleeping_; }
    uint16_t getOnValue(uint8_t ch) const { return ch < 16 ? pwm_on_[ch] : 0; }
    uint16_t getOffValue(uint8_t ch) const { return ch < 16 ? pwm_off_[ch] : 0; }

private:
    uint8_t addr_;
    bool initialized_ = false;
    bool sleeping_ = false;
    bool totempole_ = true;
    float freq_ = 50.0f;
    uint32_t oscFreq_ = 25000000;
    uint16_t pwm_on_[16];
    uint16_t pwm_off_[16];
};
