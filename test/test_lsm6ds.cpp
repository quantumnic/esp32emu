// test_lsm6ds: LSM6DS 6-axis IMU mock tests
#include "Adafruit_LSM6DS.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    lsm6ds_test::reset();

    // LSM6DSO
    Adafruit_LSM6DSO imu;
    assert(imu.begin_I2C(0x6A));

    // Default accel (0, 0, 9.81)
    sensors_event_t accel, gyro, temp;
    assert(imu.getEvent(&accel, &gyro, &temp));
    assert(fabs(accel.acceleration.z - 9.81f) < 0.01f);
    assert(fabs(gyro.gyro.x) < 0.01f);
    assert(fabs(temp.temperature - 25.0f) < 0.01f);

    // Inject values
    lsm6ds_test::setAccel(1.0f, 2.0f, 3.0f);
    lsm6ds_test::setGyro(10.0f, 20.0f, 30.0f);
    lsm6ds_test::setTemperature(40.0f);
    assert(imu.getEvent(&accel, &gyro, &temp));
    assert(fabs(accel.acceleration.x - 1.0f) < 0.01f);
    assert(fabs(accel.acceleration.y - 2.0f) < 0.01f);
    assert(fabs(gyro.gyro.z - 30.0f) < 0.01f);
    assert(fabs(temp.temperature - 40.0f) < 0.01f);

    // Config
    imu.setAccelRange(LSM6DS_ACCEL_RANGE_16_G);
    assert(imu.getAccelRange() == LSM6DS_ACCEL_RANGE_16_G);
    imu.setGyroRange(LSM6DS_GYRO_RANGE_2000_DPS);
    assert(imu.getGyroRange() == LSM6DS_GYRO_RANGE_2000_DPS);
    imu.setAccelDataRate(LSM6DS_RATE_416_HZ);
    assert(imu.getAccelDataRate() == LSM6DS_RATE_416_HZ);

    // Partial read (null pointers)
    assert(imu.getEvent(&accel, nullptr, nullptr));
    assert(imu.getEvent(nullptr, &gyro, nullptr));
    assert(imu.getEvent(nullptr, nullptr, &temp));

    // SPI init
    lsm6ds_test::reset();
    Adafruit_LSM6DSOX imu2;
    assert(imu2.begin_SPI(10));

    // Unavailable
    lsm6ds_test::reset();
    lsm6ds_test::available() = false;
    Adafruit_LSM6DS33 imu3;
    assert(!imu3.begin_I2C());

    printf("test_lsm6ds: all assertions passed\n");
    return 0;
}
