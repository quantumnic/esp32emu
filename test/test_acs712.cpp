#include "ACS712.h"
#include <cassert>
#include <cstdio>
#include <cmath>

static bool near(float a, float b, float eps = 0.01f) {
    return std::fabs(a - b) < eps;
}

int main() {
    // Test 1: Initialization
    {
        ACS712 sensor(ACS712::ACS712_20A, 34);
        assert(!sensor.isInitialized());
        sensor.begin();
        assert(sensor.isInitialized());
        assert(sensor.getPin() == 34);
        assert(sensor.getType() == ACS712::ACS712_20A);
    }

    // Test 2: Sensitivity per variant
    {
        ACS712 s5(ACS712::ACS712_05B, 34);
        assert(near(s5.getSensitivity(), 185.0f));

        ACS712 s20(ACS712::ACS712_20A, 34);
        assert(near(s20.getSensitivity(), 100.0f));

        ACS712 s30(ACS712::ACS712_30A, 34);
        assert(near(s30.getSensitivity(), 66.0f));
    }

    // Test 3: DC current reading
    {
        ACS712 sensor(ACS712::ACS712_20A, 34);
        sensor.begin();
        sensor.setCurrentDC(5.0f);
        assert(near(sensor.getCurrentDC(), 5.0f));
    }

    // Test 4: AC current reading
    {
        ACS712 sensor(ACS712::ACS712_20A, 34);
        sensor.begin();
        sensor.setCurrentAC(3.5f);
        assert(near(sensor.getCurrentAC(), 3.5f));
        assert(near(sensor.getCurrentAC(50), 3.5f));
    }

    // Test 5: Calibration
    {
        ACS712 sensor(ACS712::ACS712_20A, 34);
        sensor.begin();
        sensor.calibrate();
        assert(near(sensor.getMidPoint(), 2.5f));
    }

    // Test 6: Custom midpoint
    {
        ACS712 sensor(ACS712::ACS712_05B, 34);
        sensor.begin();
        sensor.setMidPoint(1.65f);
        assert(near(sensor.getMidPoint(), 1.65f));
    }

    // Test 7: Voltage calculation
    {
        ACS712 sensor(ACS712::ACS712_20A, 34);
        sensor.begin();
        sensor.setCurrentDC(2.0f);
        // voltage = midpoint + current * (sensitivity/1000)
        // = 2.5 + 2.0 * 0.1 = 2.7
        assert(near(sensor.getVoltage(), 2.7f));
    }

    // Test 8: Custom sensitivity
    {
        ACS712 sensor(ACS712::ACS712_20A, 34);
        sensor.begin();
        sensor.setSensitivity(50.0f);
        assert(near(sensor.getSensitivity(), 50.0f));
    }

    printf("test_acs712: all assertions passed\n");
    return 0;
}
