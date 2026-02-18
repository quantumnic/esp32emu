#include "Stepper.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    // 4-wire constructor
    Stepper motor(200, 8, 9, 10, 11);
    assert(motor.getStepsPerRevolution() == 200);
    assert(motor.getNumPins() == 4);
    assert(motor.getPin(0) == 8);
    assert(motor.getPin(3) == 11);
    assert(motor.getPosition() == 0);

    // Set speed
    motor.setSpeed(60);
    assert(motor.getSpeed() == 60);

    // Step forward
    motor.step(100);
    assert(motor.getPosition() == 100);
    assert(motor.getTotalSteps() == 100);

    // Step backward
    motor.step(-50);
    assert(motor.getPosition() == 50);
    assert(motor.getTotalSteps() == 150);

    // Full revolution
    motor.resetPosition();
    motor.step(200);
    assert(motor.getPosition() == 200);
    assert(std::fabs(motor.getAngle()) < 0.01); // 200 steps = 360° = 0°

    // Half revolution
    motor.resetPosition();
    motor.step(100);
    assert(std::fabs(motor.getAngle() - 180.0) < 0.01);

    // 2-wire constructor
    Stepper motor2(48, 2, 3);
    assert(motor2.getNumPins() == 2);
    assert(motor2.getPin(2) == -1);

    printf("test_stepper: all assertions passed\n");
    return 0;
}
