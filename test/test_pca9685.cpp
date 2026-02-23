#include "PCA9685.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    // Init
    {
        PCA9685 pwm(0x40);
        assert(pwm.begin());
        assert(pwm.isInitialized());
    }

    // PWM freq
    {
        PCA9685 pwm;
        pwm.begin();
        pwm.setPWMFreq(60.0f);
        assert(std::fabs(pwm.getPWMFreq() - 60.0f) < 0.01f);
    }

    // Set/get PWM
    {
        PCA9685 pwm;
        pwm.begin();
        pwm.setPWM(0, 0, 2048);
        assert(pwm.getOnTime(0) == 0);
        assert(pwm.getOffTime(0) == 2048);
        assert(pwm.getPWM(0, true) == 2048);
        assert(pwm.getPWM(0, false) == 0);
    }

    // setPin
    {
        PCA9685 pwm;
        pwm.begin();
        pwm.setPin(5, 1000);
        assert(pwm.getOffTime(5) == 1000);

        pwm.setPin(5, 1000, true); // invert
        assert(pwm.getOffTime(5) == 3095);
    }

    // Sleep/wake
    {
        PCA9685 pwm;
        pwm.begin();
        assert(!pwm.isSleeping());
        pwm.sleep();
        assert(pwm.isSleeping());
        pwm.wakeup();
        assert(!pwm.isSleeping());
    }

    // Reset
    {
        PCA9685 pwm;
        pwm.begin();
        pwm.setPWM(0, 100, 2000);
        pwm.reset();
        assert(pwm.getOnTime(0) == 0);
        assert(pwm.getOffTime(0) == 0);
    }

    // writeMicroseconds
    {
        PCA9685 pwm;
        pwm.begin();
        pwm.setPWMFreq(50.0f);
        pwm.writeMicroseconds(0, 1500);
        uint16_t off = pwm.getOffTime(0);
        assert(off > 200 && off < 400); // ~307 ticks for 1500us at 50Hz
    }

    // Oscillator frequency
    {
        PCA9685 pwm;
        pwm.setOscillatorFrequency(27000000);
        assert(pwm.getOscillatorFrequency() == 27000000);
    }

    printf("test_pca9685: all assertions passed\n");
    return 0;
}
