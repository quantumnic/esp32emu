#include "MAX44009.h"
#include <cassert>
#include <cstdio>
#include <cmath>

static bool near(float a, float b, float eps = 0.01f) {
    return std::fabs(a - b) < eps;
}

int main() {
    // Test 1: Init
    {
        MAX44009 sensor;
        assert(!sensor.isInitialized());
        assert(sensor.begin());
        assert(sensor.isInitialized());
        assert(sensor.getAddress() == 0x4A);
    }

    // Test 2: Custom address
    {
        MAX44009 sensor(0x4B);
        sensor.begin();
        assert(sensor.getAddress() == 0x4B);
    }

    // Test 3: Lux reading
    {
        MAX44009 sensor;
        sensor.begin();
        sensor.setLux(1234.5f);
        assert(near(sensor.getLux(), 1234.5f));
    }

    // Test 4: Configuration
    {
        MAX44009 sensor;
        sensor.begin();
        assert(!sensor.getManualMode());
        sensor.setManualMode(true);
        assert(sensor.getManualMode());

        assert(sensor.getContinuousMode());
        sensor.setContinuousMode(false);
        assert(!sensor.getContinuousMode());
    }

    // Test 5: Integration time
    {
        MAX44009 sensor;
        sensor.begin();
        sensor.setIntegrationTime(MAX44009::INT_TIME_100MS);
        assert(sensor.getIntegrationTime() == MAX44009::INT_TIME_100MS);
    }

    // Test 6: Thresholds and interrupt
    {
        MAX44009 sensor;
        sensor.begin();
        sensor.setHighThreshold(1000.0f);
        sensor.setLowThreshold(10.0f);
        assert(near(sensor.getHighThreshold(), 1000.0f));
        assert(near(sensor.getLowThreshold(), 10.0f));

        sensor.setLux(500.0f);
        assert(!sensor.getInterruptStatus());

        sensor.setLux(1500.0f);
        assert(sensor.getInterruptStatus());

        sensor.setLux(5.0f);
        assert(sensor.getInterruptStatus());
    }

    // Test 7: Interrupt enable
    {
        MAX44009 sensor;
        sensor.begin();
        assert(!sensor.isInterruptEnabled());
        sensor.enableInterrupt(true);
        assert(sensor.isInterruptEnabled());
    }

    // Test 8: Error code
    {
        MAX44009 sensor;
        sensor.begin();
        assert(sensor.getError() == 0);
        sensor.setError(1);
        assert(sensor.getError() == 1);
    }

    // Test 9: Zero lux (darkness)
    {
        MAX44009 sensor;
        sensor.begin();
        assert(near(sensor.getLux(), 0.0f));
    }

    // Test 10: Threshold timer
    {
        MAX44009 sensor;
        sensor.begin();
        sensor.setThresholdTimer(5);
        assert(sensor.getThresholdTimer() == 5);
    }

    printf("test_max44009: all assertions passed\n");
    return 0;
}
