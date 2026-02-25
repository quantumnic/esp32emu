#include "A4988.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    // Basic constructor: 200 steps/rev, DIR=2, STEP=3
    A4988 stepper(200, 2, 3);
    assert(!stepper.emu_begun());

    stepper.begin(120, 16);  // 120 RPM, 1/16 microstepping
    assert(stepper.emu_begun());
    assert(stepper.getRPM() == 120.0f);
    assert(stepper.getMicrostep() == 16);

    // Pin check
    assert(stepper.emu_dirPin() == 2);
    assert(stepper.emu_stepPin() == 3);

    // Move forward
    stepper.move(200);
    assert(stepper.emu_position() == 200);
    assert(stepper.emu_totalSteps() == 200);
    assert(stepper.emu_direction() == 1);

    // Move backward
    stepper.move(-100);
    assert(stepper.emu_position() == 100);
    assert(stepper.emu_totalSteps() == 300);
    assert(stepper.emu_direction() == -1);

    // Rotate by degrees (200 steps/rev * 16 microsteps = 3200 steps/rev)
    stepper.emu_reset();
    stepper.rotate(90.0);  // quarter turn
    long expected = (long)(90.0 * 200 * 16 / 360.0);  // 800
    assert(stepper.emu_position() == expected);

    // RPM change
    stepper.setRPM(60);
    assert(stepper.getRPM() == 60.0f);

    // Microstep change
    stepper.setMicrostep(8);
    assert(stepper.getMicrostep() == 8);

    // Enable/disable
    stepper.disable();
    assert(!stepper.emu_enabled());
    stepper.enable();
    assert(stepper.emu_enabled());

    // Speed profile
    stepper.setSpeedProfile(A4988::LINEAR_SPEED);

    // Full constructor with enable pin
    A4988 s2(200, 2, 3, 4, 5, 6, 7);
    s2.begin();
    assert(s2.emu_begun());

    printf("test_a4988: all assertions passed\n");
    return 0;
}
