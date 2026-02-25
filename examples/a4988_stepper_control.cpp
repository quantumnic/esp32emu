// esp32emu example: A4988 stepper motor driver
// Controls a NEMA17 stepper with microstepping
#include "A4988.h"

#define DIR_PIN   2
#define STEP_PIN  3
#define ENABLE_PIN 4
#define MS1_PIN   5
#define MS2_PIN   6
#define MS3_PIN   7
#define STEPS_PER_REV 200

A4988 stepper(STEPS_PER_REV, DIR_PIN, STEP_PIN, ENABLE_PIN, MS1_PIN, MS2_PIN, MS3_PIN);

void setup() {
    Serial.begin(115200);
    Serial.println("A4988 Stepper Motor Controller");

    stepper.begin(120, 16);  // 120 RPM, 1/16 microstepping
    stepper.enable();
    stepper.setSpeedProfile(A4988::LINEAR_SPEED);

    Serial.printf("RPM: %.0f, Microsteps: %d\n", stepper.getRPM(), stepper.getMicrostep());
}

void loop() {
    Serial.println("Rotating 360° CW...");
    stepper.rotate(360.0);
    delay(2000);

    Serial.println("Rotating 360° CCW...");
    stepper.rotate(-360.0);
    delay(2000);

    Serial.println("Moving 800 steps forward...");
    stepper.move(800);
    delay(1000);

    Serial.println("Quarter turn...");
    stepper.rotate(90.0);
    delay(1000);

    // Change speed
    stepper.setRPM(60);
    stepper.setMicrostep(8);
    Serial.printf("Changed to RPM: %.0f, Microsteps: %d\n", stepper.getRPM(), stepper.getMicrostep());

    stepper.rotate(180.0);
    delay(3000);

    // Reset speed
    stepper.setRPM(120);
    stepper.setMicrostep(16);
}
