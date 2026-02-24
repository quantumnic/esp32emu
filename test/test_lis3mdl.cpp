#include "Arduino.h"
#include "Wire.h"
#include "Adafruit_LIS3MDL.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    Adafruit_LIS3MDL lis;
    assert(lis.begin_I2C());
    assert(lis.test_isStarted());

    // Range
    lis.setRange(LIS3MDL_RANGE_8_GAUSS);
    assert(lis.getRange() == LIS3MDL_RANGE_8_GAUSS);

    // Data rate
    lis.setDataRate(LIS3MDL_DATARATE_80_HZ);
    assert(lis.getDataRate() == LIS3MDL_DATARATE_80_HZ);

    // Operation mode
    lis.setOperationMode(LIS3MDL_SINGLEMODE);
    assert(lis.getOperationMode() == LIS3MDL_SINGLEMODE);

    // Performance mode
    lis.setPerformanceMode(LIS3MDL_HIGHMODE);
    assert(lis.getPerformanceMode() == LIS3MDL_HIGHMODE);

    // Set magnetic field
    lis.test_setMag(0.25f, -0.15f, 0.45f);
    lis.read();
    assert(fabs(lis.x_gauss - 0.25f) < 0.01f);
    assert(fabs(lis.y_gauss - (-0.15f)) < 0.01f);
    assert(fabs(lis.z_gauss - 0.45f) < 0.01f);

    // Heading calculation
    float heading = atan2(lis.y_gauss, lis.x_gauss) * 180.0f / M_PI;
    if (heading < 0) heading += 360.0f;
    assert(heading > 300.0f && heading < 360.0f); // roughly NNW

    // Interrupt config
    lis.setIntThreshold(500);
    assert(lis.getIntThreshold() == 500);
    lis.configInterrupt(true, true, true, false, true, true);
    assert(lis.test_isIntEnabled());

    // Self test
    assert(lis.selfTest());

    printf("test_lis3mdl: all assertions passed\n");
    return 0;
}
