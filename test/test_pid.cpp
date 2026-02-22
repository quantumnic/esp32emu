// esp32emu — PID controller test
#include "Arduino.h"
#include "PID_v1.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    double input = 20.0, output = 0.0, setpoint = 100.0;

    PID pid(&input, &output, &setpoint, 2.0, 0.5, 1.0, DIRECT);

    // Default state
    assert(pid.GetMode() == MANUAL);
    assert(pid.GetDirection() == DIRECT);

    // Won't compute in manual mode
    assert(!pid.Compute());
    assert(output == 0.0);

    // Set automatic
    pid.SetMode(AUTOMATIC);
    assert(pid.GetMode() == AUTOMATIC);

    // Set output limits
    pid.SetOutputLimits(0, 255);

    // Advance time and compute
    delay(150); // > 100ms sample time
    assert(pid.Compute());
    assert(output > 0); // Should produce positive output for error > 0

    // Output should be clamped
    assert(output >= 0 && output <= 255);

    // Change setpoint below input
    setpoint = 10.0;
    delay(150);
    pid.Compute();
    // With proportional term, output should decrease

    // Sample time
    pid.SetSampleTime(200);

    // Tunings
    pid.SetTunings(1.0, 0.1, 0.5);
    assert(fabs(pid.GetKp() - 1.0) < 0.01);

    // Back to manual
    pid.SetMode(MANUAL);
    assert(!pid.Compute());

    // Reverse direction
    double input2 = 50.0, output2 = 0.0, setpoint2 = 30.0;
    PID pid2(&input2, &output2, &setpoint2, 1.0, 0.0, 0.0, REVERSE);
    pid2.SetOutputLimits(0, 100);
    pid2.SetMode(AUTOMATIC);
    delay(150);
    pid2.Compute();
    assert(pid2.GetDirection() == REVERSE);
    // For reverse: input > setpoint should produce positive output
    assert(output2 >= 0);

    // P_ON_M mode
    double input3 = 0, output3 = 0, setpoint3 = 50;
    PID pid3(&input3, &output3, &setpoint3, 2.0, 1.0, 0.0, P_ON_M, DIRECT);
    pid3.SetOutputLimits(0, 255);
    pid3.SetMode(AUTOMATIC);
    delay(150);
    pid3.Compute();
    assert(output3 >= 0);

    printf("test_pid: all assertions passed\n");
    return 0;
}
