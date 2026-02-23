#include "BNO08x.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    BNO08x imu;

    assert(imu.begin_I2C());
    assert(imu._test_isInitialized());

    // Enable reports
    assert(imu.enableReport(SH2_ROTATION_VECTOR));
    assert(imu.enableReport(SH2_ACCELEROMETER));
    assert(imu._test_reportEnabled(SH2_ROTATION_VECTOR));
    assert(imu._test_reportEnabled(SH2_ACCELEROMETER));
    assert(!imu._test_reportEnabled(SH2_GYROSCOPE_CALIBRATED));

    // No data yet
    sh2_SensorValue_t val{};
    assert(!imu.getSensorEvent(&val));

    // Inject rotation vector event
    sh2_SensorValue_t rv{};
    rv.sensorId = SH2_ROTATION_VECTOR;
    rv.un_rotationVector = {0.0f, 0.0f, 0.707f, 0.707f, 0.95f};
    imu._test_injectEvent(rv);
    assert(imu.getSensorEvent(&val));
    assert(val.sensorId == SH2_ROTATION_VECTOR);
    assert(fabsf(val.un_rotationVector.k - 0.707f) < 0.001f);
    assert(fabsf(val.un_rotationVector.real - 0.707f) < 0.001f);

    // Consumed
    assert(!imu.getSensorEvent(&val));

    // Inject accelerometer event
    sh2_SensorValue_t acc{};
    acc.sensorId = SH2_ACCELEROMETER;
    acc.un_accelerometer = {0.1f, -0.2f, 9.8f};
    imu._test_injectEvent(acc);
    assert(imu.getSensorEvent(&val));
    assert(fabsf(val.un_accelerometer.z - 9.8f) < 0.01f);

    // Reset detection
    assert(!imu.wasReset());
    imu._test_setReset(true);
    assert(imu.wasReset());
    assert(!imu.wasReset()); // cleared after read

    printf("test_bno08x: all assertions passed\n");
    return 0;
}
