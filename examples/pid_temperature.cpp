// esp32emu example — PID Temperature Controller
// Simulate a heater controlling temperature with PID.
#include "Arduino.h"
#include "PID_v1.h"

double input = 22.0;    // Current temperature
double output = 0.0;     // Heater PWM
double setpoint = 50.0;  // Target temperature

PID myPID(&input, &output, &setpoint, 2.0, 0.5, 1.0, DIRECT);

void setup() {
    Serial.begin(115200);
    Serial.println("PID Temperature Controller");
    Serial.printf("Setpoint: %.1f°C\n", setpoint);

    myPID.SetMode(AUTOMATIC);
    myPID.SetOutputLimits(0, 255);
    myPID.SetSampleTime(100);
}

void loop() {
    myPID.Compute();

    // Simulate heating/cooling
    input += (output / 255.0) * 0.5;  // Heater adds heat
    input -= (input - 20.0) * 0.02;   // Cooling to ambient

    Serial.printf("Temp: %5.1f°C | PWM: %3.0f | Target: %.0f°C", input, output, setpoint);
    if (abs(input - setpoint) < 1.0) Serial.print(" ✓");
    Serial.println();

    delay(200);
}
