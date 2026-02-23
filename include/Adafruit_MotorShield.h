// Adafruit Motor Shield V2 mock for esp32emu
// DC motors + stepper motors via I2C PCA9685 PWM driver
#pragma once
#include <cstdint>
#include <cstdio>
#include <cmath>
#include <algorithm>

// Motor directions
#define FORWARD  1
#define BACKWARD 2
#define BRAKE    3
#define RELEASE  4

// Stepper styles
#define SINGLE     1
#define DOUBLE     2
#define INTERLEAVE 3
#define MICROSTEP  4

class Adafruit_DCMotor {
public:
    Adafruit_DCMotor() = default;

    void run(uint8_t cmd) {
        direction_ = cmd;
        const char* dir_str = "UNKNOWN";
        switch (cmd) {
            case FORWARD:  dir_str = "FORWARD"; break;
            case BACKWARD: dir_str = "BACKWARD"; break;
            case BRAKE:    dir_str = "BRAKE"; break;
            case RELEASE:  dir_str = "RELEASE"; break;
        }
        fprintf(stderr, "[esp32emu] DCMotor %d: %s (speed=%d)\n", num_, dir_str, speed_);
    }

    void setSpeed(uint8_t speed) {
        speed_ = speed;
    }

    void setSpeedFine(uint16_t speed) {
        speed_ = std::min(speed, (uint16_t)4095);
    }

    // Test helpers
    uint8_t getDirection() const { return direction_; }
    uint8_t getSpeed() const { return speed_; }
    uint8_t getNum() const { return num_; }

    void _setNum(uint8_t n) { num_ = n; }

private:
    uint8_t num_ = 0;
    uint8_t speed_ = 0;
    uint8_t direction_ = RELEASE;
};

class Adafruit_StepperMotor {
public:
    Adafruit_StepperMotor() = default;

    void setSpeed(uint16_t rpm) {
        rpm_ = rpm;
        fprintf(stderr, "[esp32emu] StepperMotor %d: speed=%d RPM\n", num_, rpm);
    }

    void step(uint16_t steps, uint8_t dir, uint8_t style = SINGLE) {
        int sign = (dir == FORWARD) ? 1 : -1;
        int multiplier = 1;
        if (style == INTERLEAVE) multiplier = 2;
        else if (style == MICROSTEP) multiplier = 16;

        position_ += sign * steps * multiplier;
        total_steps_ += steps;

        const char* style_str = "SINGLE";
        switch (style) {
            case DOUBLE:     style_str = "DOUBLE"; break;
            case INTERLEAVE: style_str = "INTERLEAVE"; break;
            case MICROSTEP:  style_str = "MICROSTEP"; break;
        }
        fprintf(stderr, "[esp32emu] StepperMotor %d: %d steps %s (%s) pos=%d\n",
                num_, steps, dir == FORWARD ? "FWD" : "BWD", style_str, position_);
    }

    void release() {
        fprintf(stderr, "[esp32emu] StepperMotor %d: released\n", num_);
    }

    // Test helpers
    int getPosition() const { return position_; }
    uint16_t getRPM() const { return rpm_; }
    uint32_t getTotalSteps() const { return total_steps_; }
    uint8_t getNum() const { return num_; }

    void _setNum(uint8_t n) { num_ = n; }
    void _setStepsPerRev(uint16_t s) { steps_per_rev_ = s; }

private:
    uint8_t num_ = 0;
    uint16_t rpm_ = 0;
    uint16_t steps_per_rev_ = 200;
    int position_ = 0;
    uint32_t total_steps_ = 0;
};

class Adafruit_MotorShield {
public:
    Adafruit_MotorShield(uint8_t addr = 0x60) : addr_(addr) {
        for (int i = 0; i < 4; i++) {
            dc_motors_[i]._setNum(i + 1);
            stepper_motors_[i / 2]._setNum(i / 2 + 1);
        }
    }

    bool begin(uint16_t freq = 1600) {
        freq_ = freq;
        begun_ = true;
        fprintf(stderr, "[esp32emu] MotorShield (0x%02X): begin, PWM freq=%dHz\n", addr_, freq);
        return true;
    }

    Adafruit_DCMotor* getMotor(uint8_t num) {
        if (num < 1 || num > 4) return nullptr;
        return &dc_motors_[num - 1];
    }

    Adafruit_StepperMotor* getStepper(uint16_t steps_per_rev, uint8_t num) {
        if (num < 1 || num > 2) return nullptr;
        stepper_motors_[num - 1]._setStepsPerRev(steps_per_rev);
        return &stepper_motors_[num - 1];
    }

    void setPWM(uint8_t pin, uint16_t val) {
        if (pin < 16) pwm_vals_[pin] = val;
    }

    void setPin(uint8_t pin, bool val) {
        if (pin < 16) pwm_vals_[pin] = val ? 4095 : 0;
    }

    // Test helpers
    uint8_t getAddr() const { return addr_; }
    uint16_t getFreq() const { return freq_; }
    bool isBegun() const { return begun_; }
    uint16_t getPWM(uint8_t pin) const { return pin < 16 ? pwm_vals_[pin] : 0; }

private:
    uint8_t addr_;
    uint16_t freq_ = 1600;
    bool begun_ = false;
    Adafruit_DCMotor dc_motors_[4];
    Adafruit_StepperMotor stepper_motors_[2];
    uint16_t pwm_vals_[16] = {};
};
