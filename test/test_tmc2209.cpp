// esp32emu — TMC2209 stepper driver test
#include "Arduino.h"
#include "TMC2209.h"
#include <cassert>
#include <cstdio>

int main() {
    TMC2209 driver;

    // Setup
    driver.setup(Serial, 115200, 0);
    assert(driver.isSetupAndCommunicating());
    assert(driver.getVersion() == 0x21);

    // Enable/disable
    assert(!driver.isEnabled());
    driver.enable();
    assert(driver.isEnabled());
    driver.disable();
    assert(!driver.isEnabled());

    // Current
    driver.setRunCurrent(80);
    driver.setHoldCurrent(40);
    assert(driver.getRunCurrent() == 80);
    assert(driver.getHoldCurrent() == 40);

    // Microsteps
    assert(driver.getMicrostepsPerStep() == 256);
    driver.setMicrostepsPerStep(16);
    assert(driver.getMicrostepsPerStep() == 16);

    // StealthChop
    assert(driver.isStealthChopEnabled());
    driver.disableStealthChop();
    assert(!driver.isStealthChopEnabled());
    driver.enableStealthChop();
    assert(driver.isStealthChopEnabled());

    // CoolStep
    assert(!driver.isCoolStepEnabled());
    driver.enableCoolStep(2, 5);
    assert(driver.isCoolStepEnabled());
    driver.disableCoolStep();
    assert(!driver.isCoolStepEnabled());

    // Standstill mode
    assert(driver.getStandstillMode() == TMC2209::NORMAL);
    driver.setStandstillMode(TMC2209::FREEWHEELING);
    assert(driver.getStandstillMode() == TMC2209::FREEWHEELING);

    // StallGuard
    driver.setStallGuardThreshold(100);
    assert(driver.getStallGuardThreshold() == 100);
    driver._setStallGuardResult(512);
    assert(driver.getStallGuardResult() == 512);

    // Velocity
    assert(driver.isStandstill());
    driver.moveAtVelocity(50000);
    assert(driver.getVelocity() == 50000);
    assert(!driver.isStandstill());
    driver.moveAtVelocity(0);
    assert(driver.isStandstill());

    // Status
    TMC2209::Status st;
    st.over_temperature_warning = true;
    driver._setStatus(st);
    assert(driver.getStatus().over_temperature_warning);
    assert(!driver.getStatus().short_to_ground_a);

    // Communication test
    driver._setCommunicating(false);
    assert(!driver.isCommunicating());
    assert(!driver.isSetupAndCommunicating());

    printf("test_tmc2209: all assertions passed\n");
    return 0;
}
