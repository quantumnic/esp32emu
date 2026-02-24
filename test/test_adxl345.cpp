#include "Arduino.h"
#include "Wire.h"
#include "Adafruit_ADXL345_U.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    Adafruit_ADXL345_Unified accel(12345);
    assert(accel.begin());
    assert(accel.test_isStarted());

    // Range
    accel.setRange(ADXL345_RANGE_4_G);
    assert(accel.getRange() == ADXL345_RANGE_4_G);

    // Data rate
    accel.setDataRate(ADXL345_DATARATE_200_HZ);
    assert(accel.getDataRate() == ADXL345_DATARATE_200_HZ);

    // Default reading (gravity on Z)
    sensors_event_t_adxl event;
    accel.getEvent(&event);
    assert(fabs(event.acceleration.z - 9.81f) < 0.01f);
    assert(fabs(event.acceleration.x) < 0.01f);

    // Set acceleration
    accel.test_setAccel(1.5f, -2.0f, 9.5f);
    accel.getEvent(&event);
    assert(fabs(event.acceleration.x - 1.5f) < 0.01f);
    assert(fabs(event.acceleration.y - (-2.0f)) < 0.01f);
    assert(fabs(event.acceleration.z - 9.5f) < 0.01f);

    // Pitch/roll
    float pitch = accel.test_getPitch();
    float roll = accel.test_getRoll();
    assert(fabs(pitch) < 20.0f);
    assert(fabs(roll) < 20.0f);

    // Tap detection config
    accel.setTapThreshold(80);
    assert(accel.getTapThreshold() == 80);
    accel.setTapDuration(10);
    assert(accel.getTapDuration() == 10);
    accel.setDoubleTapLatency(50);
    accel.setDoubleTapWindow(200);

    // Free-fall config
    accel.setFreeFallThreshold(7);
    accel.setFreeFallDuration(30);

    // Activity/inactivity
    accel.setActivityThreshold(75);
    accel.setInactivityThreshold(15);
    accel.setInactivityTime(10);

    // Interrupt source
    accel.test_setInterruptSource(0x42);
    assert(accel.getInterruptSource() == 0x42);

    printf("test_adxl345: all assertions passed\n");
    return 0;
}
