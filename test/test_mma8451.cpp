#include "Adafruit_MMA8451.h"
#include <cassert>
#include <cstdio>
#include <cmath>

static bool near(float a, float b, float eps = 0.01f) {
    return std::fabs(a - b) < eps;
}

int main() {
    // Test 1: Init
    {
        Adafruit_MMA8451 accel;
        assert(!accel.isInitialized());
        assert(accel.begin());
        assert(accel.isInitialized());
        assert(accel.getAddress() == 0x1D);
    }

    // Test 2: Custom address
    {
        Adafruit_MMA8451 accel(0x1C);
        accel.begin();
        assert(accel.getAddress() == 0x1C);
    }

    // Test 3: Set/get range
    {
        Adafruit_MMA8451 accel;
        accel.begin();
        assert(accel.getRange() == MMA8451_RANGE_2_G);
        accel.setRange(MMA8451_RANGE_8_G);
        assert(accel.getRange() == MMA8451_RANGE_8_G);
    }

    // Test 4: Data rate
    {
        Adafruit_MMA8451 accel;
        accel.begin();
        accel.setDataRate(MMA8451_DATARATE_50_HZ);
        assert(accel.getDataRate() == MMA8451_DATARATE_50_HZ);
    }

    // Test 5: Set acceleration
    {
        Adafruit_MMA8451 accel;
        accel.begin();
        accel.setAcceleration(0.0f, 0.0f, 1.0f);
        assert(near(accel.x_g, 0.0f));
        assert(near(accel.y_g, 0.0f));
        assert(near(accel.z_g, 1.0f));
    }

    // Test 6: Raw values at 2G range
    {
        Adafruit_MMA8451 accel;
        accel.begin();
        accel.setRange(MMA8451_RANGE_2_G);
        accel.setAcceleration(1.0f, 0.0f, 0.0f);
        assert(accel.x == 4096); // 1g * 4096 counts/g
        assert(accel.y == 0);
    }

    // Test 7: Raw values at 8G range
    {
        Adafruit_MMA8451 accel;
        accel.begin();
        accel.setRange(MMA8451_RANGE_8_G);
        accel.setAcceleration(1.0f, 0.0f, 0.0f);
        assert(accel.x == 1024); // 1g * 1024 counts/g
    }

    // Test 8: Orientation
    {
        Adafruit_MMA8451 accel;
        accel.begin();
        accel.setAcceleration(0.0f, 0.0f, 1.0f);
        assert(accel.getOrientation() == 0); // z dominant = portrait up

        accel.setAcceleration(1.0f, 0.0f, 0.0f);
        assert(accel.getOrientation() == 2); // x+ = landscape right

        accel.setAcceleration(-1.0f, 0.0f, 0.0f);
        assert(accel.getOrientation() == 3); // x- = landscape left
    }

    // Test 9: read() doesn't crash
    {
        Adafruit_MMA8451 accel;
        accel.begin();
        accel.read(); // should be a no-op in mock
    }

    printf("test_mma8451: all assertions passed\n");
    return 0;
}
