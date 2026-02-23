#include "ICM20948.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    ICM_20948_I2C imu;

    assert(imu.begin());
    assert(imu._test_isInitialized());

    // Configure
    assert(imu.setFullScale(ICM_20948_I2C::gpm4));
    assert(imu.setFullScale(ICM_20948_I2C::dps500));

    // Inject accel
    imu._test_setAccel(0.5f, -1.0f, 9.81f);
    assert(imu.dataReady());
    assert(imu.getAGMT());
    assert(fabsf(imu.accX() - 0.5f) < 0.01f);
    assert(fabsf(imu.accZ() - 9.81f) < 0.01f);

    // Inject gyro
    imu._test_setGyro(10.0f, -5.0f, 0.5f);
    assert(imu.dataReady());
    imu.getAGMT();
    assert(fabsf(imu.gyrX() - 10.0f) < 0.01f);

    // Inject mag
    imu._test_setMag(30.0f, -10.0f, 45.0f);
    imu.getAGMT();
    assert(fabsf(imu.magX() - 30.0f) < 0.01f);
    assert(fabsf(imu.magZ() - 45.0f) < 0.01f);

    // Temperature
    imu._test_setTemp(36.5f);
    imu.getAGMT();
    assert(fabsf(imu.temp() - 36.5f) < 0.01f);

    // Sleep
    assert(imu.sleep(true));
    assert(imu._test_isSleeping());
    assert(imu.sleep(false));
    assert(!imu._test_isSleeping());

    printf("test_icm20948: all assertions passed\n");
    return 0;
}
