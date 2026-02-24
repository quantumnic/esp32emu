#include "Adafruit_MPRLS.h"
#include <cassert>
#include <cstdio>
#include <cmath>

static bool near(float a, float b, float eps = 0.01f) {
    return std::fabs(a - b) < eps;
}

int main() {
    // Test 1: Init
    {
        Adafruit_MPRLS mpr;
        assert(mpr.begin(0x18));
    }

    // Test 2: Default pressure
    {
        Adafruit_MPRLS mpr;
        mpr.begin();
        assert(near(mpr.readPressure(), 1013.25f));
    }

    // Test 3: Test helpers
    {
        Adafruit_MPRLS mpr;
        mpr.begin();
        mpr._test_set_pressure(800.0f);
        assert(near(mpr.readPressure(), 800.0f));
    }

    // Test 4: Status
    {
        Adafruit_MPRLS mpr;
        mpr.begin();
        assert(mpr.lastStatus() == 0x40);
        mpr._test_set_status(0x00);
        assert(mpr.lastStatus() == 0x00);
    }

    // Test 5: Constructor with pins
    {
        Adafruit_MPRLS mpr(4, 5, 0, 25);
        assert(mpr.begin());
        assert(near(mpr.readPressure(), 1013.25f));
    }

    printf("test_mprls: all assertions passed\n");
    return 0;
}
