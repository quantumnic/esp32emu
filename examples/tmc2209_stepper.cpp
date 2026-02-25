// esp32emu example — TMC2209 Stepper Motor Driver via UART
#include <Arduino.h>
#include <TMCStepper.h>

#define STEP_PIN   25
#define DIR_PIN    26
#define EN_PIN     27
#define R_SENSE    0.11f

TMC2209Stepper driver(&Serial1, R_SENSE, 0);

void setup() {
    Serial.begin(115200);
    Serial1.begin(115200);

    pinMode(STEP_PIN, OUTPUT);
    pinMode(DIR_PIN, OUTPUT);
    pinMode(EN_PIN, OUTPUT);
    digitalWrite(EN_PIN, LOW); // Enable driver

    driver.begin();
    driver.rms_current(800);     // 800mA RMS
    driver.microsteps(16);       // 16 microsteps
    driver.en_spreadCycle(false); // StealthChop mode
    driver.SGTHRS(50);           // StallGuard threshold

    Serial.print("Connection: ");
    Serial.println(driver.test_connection() == 0 ? "OK" : "FAIL");
    Serial.print("Microsteps: ");
    Serial.println(driver.microsteps());
    Serial.println("TMC2209 configured.");
}

void loop() {
    // Step the motor
    digitalWrite(DIR_PIN, HIGH);
    for (int i = 0; i < 200 * 16; i++) { // One full rotation at 16 microsteps
        digitalWrite(STEP_PIN, HIGH);
        delayMicroseconds(100);
        digitalWrite(STEP_PIN, LOW);
        delayMicroseconds(100);
    }

    Serial.print("StallGuard: ");
    Serial.println(driver.SG_RESULT());

    delay(1000);
}
