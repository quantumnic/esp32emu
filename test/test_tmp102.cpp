#include "TMP102.h"
#include <cassert>
#include <cstdio>
#include <cmath>

static bool near(float a, float b, float eps = 0.01f) {
    return std::fabs(a - b) < eps;
}

int main() {
    // Test 1: Init
    {
        TMP102 sensor;
        assert(!sensor.isInitialized());
        assert(sensor.begin());
        assert(sensor.isInitialized());
        assert(sensor.getAddress() == 0x48);
    }

    // Test 2: Custom address
    {
        TMP102 sensor(0x49);
        sensor.begin();
        assert(sensor.getAddress() == 0x49);
    }

    // Test 3: Temperature reading
    {
        TMP102 sensor;
        sensor.begin();
        sensor.setTemperatureC(22.5f);
        assert(near(sensor.readTempC(), 22.5f));
        assert(near(sensor.readTempF(), 72.5f));
    }

    // Test 4: Sleep/wake
    {
        TMP102 sensor;
        sensor.begin();
        assert(!sensor.isSleeping());
        sensor.sleep();
        assert(sensor.isSleeping());
        sensor.wakeup();
        assert(!sensor.isSleeping());
    }

    // Test 5: Alert thresholds
    {
        TMP102 sensor;
        sensor.begin();
        sensor.setHighTempC(50.0f);
        sensor.setLowTempC(10.0f);
        assert(near(sensor.getHighTempC(), 50.0f));
        assert(near(sensor.getLowTempC(), 10.0f));

        sensor.setTemperatureC(25.0f);
        assert(!sensor.alert());

        sensor.setTemperatureC(55.0f);
        assert(sensor.alert());

        sensor.setTemperatureC(5.0f);
        assert(sensor.alert());
    }

    // Test 6: Extended mode
    {
        TMP102 sensor;
        sensor.begin();
        assert(!sensor.getExtendedMode());
        sensor.setExtendedMode(true);
        assert(sensor.getExtendedMode());
    }

    // Test 7: Conversion rate
    {
        TMP102 sensor;
        sensor.begin();
        assert(sensor.getConversionRate() == 2);
        sensor.setConversionRate(3);
        assert(sensor.getConversionRate() == 3);
    }

    // Test 8: Negative temperature
    {
        TMP102 sensor;
        sensor.begin();
        sensor.setTemperatureC(-10.5f);
        assert(near(sensor.readTempC(), -10.5f));
        assert(near(sensor.readTempF(), 13.1f));
    }

    printf("test_tmp102: all assertions passed\n");
    return 0;
}
