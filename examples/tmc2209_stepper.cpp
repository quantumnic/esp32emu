// esp32emu example — TMC2209 Stepper Motor Driver
// Configure and control a TMC2209 via UART.
#include "Arduino.h"
#include "TMC2209.h"

TMC2209 driver;

void setup() {
    Serial.begin(115200);
    Serial.println("TMC2209 Stepper Driver Demo");

    driver.setup(Serial1, 115200, 0);

    if (driver.isSetupAndCommunicating()) {
        Serial.printf("TMC2209 connected, version: 0x%02X\n", driver.getVersion());
    }

    // Configure
    driver.setRunCurrent(50);
    driver.setHoldCurrent(25);
    driver.setMicrostepsPerStep(16);
    driver.enableStealthChop();
    driver.setStandstillMode(TMC2209::FREEWHEELING);
    driver.setStallGuardThreshold(50);

    Serial.printf("Run current: %d%%\n", driver.getRunCurrent());
    Serial.printf("Microsteps: %d\n", driver.getMicrostepsPerStep());
    Serial.printf("StealthChop: %s\n", driver.isStealthChopEnabled() ? "ON" : "OFF");

    // Enable and move
    driver.enable();
    driver.moveAtVelocity(10000);
    Serial.printf("Moving at velocity: %d\n", (int)driver.getVelocity());

    delay(2000);

    driver.moveAtVelocity(0);
    Serial.println("Stopped");
    Serial.printf("Standstill: %s\n", driver.isStandstill() ? "yes" : "no");
}

void loop() { delay(1000); }
