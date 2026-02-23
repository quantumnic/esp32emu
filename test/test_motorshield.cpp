// test_motorshield: Adafruit Motor Shield V2 mock tests
#include "Adafruit_MotorShield.h"
#include <cassert>
#include <cstdio>

int main() {
    Adafruit_MotorShield shield(0x60);
    assert(!shield.isBegun());
    assert(shield.begin());
    assert(shield.isBegun());
    assert(shield.getAddr() == 0x60);
    assert(shield.getFreq() == 1600);

    // Custom address + frequency
    Adafruit_MotorShield shield2(0x61);
    shield2.begin(800);
    assert(shield2.getAddr() == 0x61);
    assert(shield2.getFreq() == 800);

    // DC Motor
    auto* m1 = shield.getMotor(1);
    assert(m1 != nullptr);
    assert(m1->getNum() == 1);
    assert(m1->getDirection() == RELEASE);

    m1->setSpeed(200);
    assert(m1->getSpeed() == 200);
    m1->run(FORWARD);
    assert(m1->getDirection() == FORWARD);
    m1->run(BACKWARD);
    assert(m1->getDirection() == BACKWARD);
    m1->run(BRAKE);
    assert(m1->getDirection() == BRAKE);
    m1->run(RELEASE);
    assert(m1->getDirection() == RELEASE);

    // All 4 DC motors
    for (int i = 1; i <= 4; i++) {
        auto* m = shield.getMotor(i);
        assert(m != nullptr);
        assert(m->getNum() == i);
    }
    assert(shield.getMotor(0) == nullptr);
    assert(shield.getMotor(5) == nullptr);

    // Stepper Motor
    auto* s1 = shield.getStepper(200, 1);
    assert(s1 != nullptr);
    assert(s1->getNum() == 1);
    s1->setSpeed(60);
    assert(s1->getRPM() == 60);

    s1->step(100, FORWARD, SINGLE);
    assert(s1->getPosition() == 100);
    assert(s1->getTotalSteps() == 100);

    s1->step(50, BACKWARD, SINGLE);
    assert(s1->getPosition() == 50);
    assert(s1->getTotalSteps() == 150);

    s1->step(10, FORWARD, DOUBLE);
    assert(s1->getPosition() == 60);

    // Interleave doubles effective steps
    s1->step(10, FORWARD, INTERLEAVE);
    assert(s1->getPosition() == 80);

    // Microstep 16x
    s1->step(1, FORWARD, MICROSTEP);
    assert(s1->getPosition() == 96);

    s1->release();

    // Stepper 2
    auto* s2 = shield.getStepper(200, 2);
    assert(s2 != nullptr);
    assert(s2->getNum() == 2);
    assert(shield.getStepper(200, 3) == nullptr);

    // PWM pin control
    shield.setPWM(0, 2048);
    assert(shield.getPWM(0) == 2048);
    shield.setPin(1, true);
    assert(shield.getPWM(1) == 4095);
    shield.setPin(1, false);
    assert(shield.getPWM(1) == 0);

    printf("test_motorshield: all assertions passed\n");
    return 0;
}
