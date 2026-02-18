/*
 * Stepper Sweep — Rotate a stepper motor back and forth.
 * Uses a 200 steps/rev motor (1.8° per step) at 30 RPM.
 */

#include <Stepper.h>

const int stepsPerRev = 200;
Stepper motor(stepsPerRev, 8, 9, 10, 11);

void setup() {
    Serial.begin(115200);
    motor.setSpeed(30);
    Serial.println("Stepper sweep demo");
}

int direction = 1;
int cycle = 0;

void loop() {
    int steps = stepsPerRev / 2 * direction;
    motor.step(steps);
    Serial.print("Cycle ");
    Serial.print(cycle);
    Serial.print(": moved ");
    Serial.print(steps);
    Serial.println(" steps");

    direction = -direction;
    cycle++;
    delay(1000);

    if (cycle >= 6) {
        Serial.println("Done!");
        while (true) delay(1000);
    }
}
