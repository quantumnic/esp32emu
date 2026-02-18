#pragma once
#include <cstdint>
#include <cstdio>
#include <cmath>

/// @brief Arduino Stepper library mock for esp32emu.
/// Tracks position, speed, and step counts for host-side testing.
class Stepper {
public:
    /// 2-wire constructor
    Stepper(int stepsPerRevolution, int pin1, int pin2)
        : stepsPerRev_(stepsPerRevolution), pins_{pin1, pin2, -1, -1}, numPins_(2) {}

    /// 4-wire constructor
    Stepper(int stepsPerRevolution, int pin1, int pin2, int pin3, int pin4)
        : stepsPerRev_(stepsPerRevolution), pins_{pin1, pin2, pin3, pin4}, numPins_(4) {}

    /// Set speed in RPM
    void setSpeed(long rpm) { rpm_ = rpm; }

    /// Move the motor a given number of steps (positive = CW, negative = CCW)
    void step(int steps) {
        position_ += steps;
        totalSteps_ += std::abs(steps);
        fprintf(stderr, "⚙️  Stepper: %+d steps (pos=%d, %.1f RPM)\n", steps, position_, (double)rpm_);
    }

    // ── Test helpers ──
    int getPosition() const { return position_; }
    int getTotalSteps() const { return totalSteps_; }
    long getSpeed() const { return rpm_; }
    int getStepsPerRevolution() const { return stepsPerRev_; }
    int getPin(int idx) const { return (idx >= 0 && idx < numPins_) ? pins_[idx] : -1; }
    int getNumPins() const { return numPins_; }

    /// Get current angle in degrees (0-360, wraps)
    double getAngle() const {
        int pos = position_ % stepsPerRev_;
        if (pos < 0) pos += stepsPerRev_;
        return 360.0 * pos / stepsPerRev_;
    }

    void resetPosition() { position_ = 0; totalSteps_ = 0; }

private:
    int stepsPerRev_;
    int pins_[4];
    int numPins_;
    long rpm_ = 0;
    int position_ = 0;
    int totalSteps_ = 0;
};
