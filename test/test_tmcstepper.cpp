// esp32emu test — TMCStepper (TMC2209)
#include <cassert>
#include <cstdio>
#define ESP32EMU_MOCK
#include "Arduino.h"
#include "TMCStepper.h"

int main() {
    // Test init
    {
        TMC2209Stepper driver(nullptr, 0.11f, 0);
        assert(!driver.test_isInitialized());
        assert(driver.begin());
        assert(driver.test_isInitialized());
        assert(driver.test_connection() == 0);
    }

    // Test connection before init
    {
        TMC2209Stepper driver(nullptr, 0.11f);
        assert(driver.test_connection() == 1);
    }

    // Test current
    {
        TMC2209Stepper driver(nullptr, 0.11f);
        driver.begin();
        driver.rms_current(800);
        assert(driver.rms_current() == 800);
    }

    // Test microsteps
    {
        TMC2209Stepper driver(nullptr, 0.11f);
        driver.begin();
        assert(driver.microsteps() == 256); // default
        driver.microsteps(16);
        assert(driver.microsteps() == 16);
    }

    // Test SpreadCycle
    {
        TMC2209Stepper driver(nullptr, 0.11f);
        driver.begin();
        assert(!driver.en_spreadCycle());
        driver.en_spreadCycle(true);
        assert(driver.en_spreadCycle());
    }

    // Test StallGuard
    {
        TMC2209Stepper driver(nullptr, 0.11f);
        driver.begin();
        driver.SGTHRS(100);
        assert(driver.SGTHRS() == 100);
        driver.test_setSGResult(500);
        assert(driver.SG_RESULT() == 500);
    }

    // Test VACTUAL
    {
        TMC2209Stepper driver(nullptr, 0.11f);
        driver.begin();
        driver.VACTUAL(10000);
        assert(driver.VACTUAL() == 10000);
        driver.VACTUAL(-5000);
        assert(driver.VACTUAL() == -5000);
    }

    // Test shaft direction
    {
        TMC2209Stepper driver(nullptr, 0.11f);
        driver.begin();
        assert(!driver.shaft());
        driver.shaft(true);
        assert(driver.shaft());
    }

    // Test pin constructor
    {
        TMC2209Stepper driver(16, 17, 0.11f, 0);
        assert(driver.begin());
        assert(driver.test_connection() == 0);
    }

    // Test TMC2208 alias
    {
        TMC2208Stepper driver(nullptr, 0.11f);
        driver.begin();
        driver.rms_current(600);
        assert(driver.rms_current() == 600);
    }

    printf("test_tmcstepper: all assertions passed\n");
    return 0;
}
