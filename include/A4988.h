#pragma once
// esp32emu: A4988 stepper motor driver mock
// Common in CNC/3D printing: STEP + DIR pins, microstepping, enable
#include "Arduino.h"
#include <cstdio>

class A4988 {
public:
    // Minimal constructor: steps/rev, dir, step
    A4988(int steps_per_rev, int dir_pin, int step_pin)
        : _stepsPerRev(steps_per_rev), _dirPin(dir_pin), _stepPin(step_pin),
          _enablePin(-1), _ms1(-1), _ms2(-1), _ms3(-1) {}

    // Full constructor with enable and microstepping pins
    A4988(int steps_per_rev, int dir_pin, int step_pin, int enable_pin,
          int ms1 = -1, int ms2 = -1, int ms3 = -1)
        : _stepsPerRev(steps_per_rev), _dirPin(dir_pin), _stepPin(step_pin),
          _enablePin(enable_pin), _ms1(ms1), _ms2(ms2), _ms3(ms3) {}

    void begin(float rpm = 60, int microsteps = 1) {
        _rpm = rpm;
        _microsteps = microsteps;
        _begun = true;
        printf("[esp32emu] A4988: initialized (rpm=%.0f, microsteps=%d)\n", rpm, microsteps);
    }

    void setRPM(float rpm) { _rpm = rpm; }
    float getRPM() const { return _rpm; }

    void setMicrostep(int microsteps) { _microsteps = microsteps; }
    int getMicrostep() const { return _microsteps; }

    void setSpeedProfile(int profile) { _profile = profile; }

    void enable() { _enabled = true; }
    void disable() { _enabled = false; }

    // Move: positive = CW, negative = CCW
    void move(long steps) {
        if (steps > 0) _direction = 1;
        else if (steps < 0) _direction = -1;
        _totalSteps += (steps > 0 ? steps : -steps);
        _position += steps;
    }

    // Rotate by degrees
    void rotate(double deg) {
        long steps = (long)(deg * _stepsPerRev * _microsteps / 360.0);
        move(steps);
    }

    // Rotate full revolutions
    void rotate(long revolutions) {
        move(revolutions * _stepsPerRev * _microsteps);
    }

    void stop() { _direction = 0; }

    // ── Test helpers ──
    long emu_position() const { return _position; }
    unsigned long emu_totalSteps() const { return _totalSteps; }
    int emu_direction() const { return _direction; }
    bool emu_begun() const { return _begun; }
    bool emu_enabled() const { return _enabled; }
    int emu_dirPin() const { return _dirPin; }
    int emu_stepPin() const { return _stepPin; }
    void emu_reset() { _position = 0; _totalSteps = 0; _direction = 0; }

    // Speed profile constants
    static constexpr int LINEAR_SPEED = 0;
    static constexpr int CONSTANT_SPEED = 1;

private:
    int _stepsPerRev;
    int _dirPin, _stepPin;
    [[maybe_unused]] int _enablePin;
    [[maybe_unused]] int _ms1, _ms2, _ms3;
    float _rpm = 60;
    int _microsteps = 1;
    int _profile = 0;
    bool _begun = false;
    bool _enabled = true;
    int _direction = 0;
    long _position = 0;
    unsigned long _totalSteps = 0;
};
