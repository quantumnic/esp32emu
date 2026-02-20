#include "Adafruit_PWMServoDriver.h"
#include <cassert>
#include <cmath>
#include <cstdio>

int main() {
    Adafruit_PWMServoDriver pwm;
    assert(!pwm.isInitialized());
    assert(pwm.begin());
    assert(pwm.isInitialized());
    assert(pwm.getAddress() == 0x40);

    // Custom address
    Adafruit_PWMServoDriver pwm2(0x41);
    pwm2.begin();
    assert(pwm2.getAddress() == 0x41);

    // Frequency
    pwm.setPWMFreq(60.0f);
    assert(std::fabs(pwm.getPWMFreq() - 60.0f) < 0.01f);

    // Set PWM
    pwm.setPWM(0, 0, 300);
    assert(pwm.getOnValue(0) == 0);
    assert(pwm.getOffValue(0) == 300);
    assert(pwm.getPWM(0, false) == 0);
    assert(pwm.getPWM(0, true) == 300);

    // Set pin full on
    pwm.setPin(1, 4095);
    assert(pwm.getOnValue(1) == 4096);
    assert(pwm.getOffValue(1) == 0);

    // Set pin full off
    pwm.setPin(2, 0);
    assert(pwm.getOnValue(2) == 0);
    assert(pwm.getOffValue(2) == 4096);

    // Set pin mid value
    pwm.setPin(3, 2048);
    assert(pwm.getOnValue(3) == 0);
    assert(pwm.getOffValue(3) == 2048);

    // Invert
    pwm.setPin(4, 1000, true);
    assert(pwm.getOffValue(4) == 3095);

    // Out of range channel
    pwm.setPWM(16, 100, 200); // should be ignored
    assert(pwm.getOnValue(16) == 0);

    // writeMicroseconds
    pwm.setPWMFreq(50.0f);
    pwm.writeMicroseconds(5, 1500); // mid servo
    assert(pwm.getOffValue(5) > 200 && pwm.getOffValue(5) < 400);

    // Reset
    pwm.reset();
    assert(pwm.getOnValue(0) == 0);
    assert(pwm.getOffValue(0) == 0);

    // Sleep/wake
    pwm.sleep();
    assert(pwm.isSleeping());
    pwm.wakeup();
    assert(!pwm.isSleeping());

    // Oscillator frequency
    pwm.setOscillatorFrequency(27000000);
    assert(pwm.getOscillatorFrequency() == 27000000);

    printf("test_pca9685: all assertions passed\n");
    return 0;
}
