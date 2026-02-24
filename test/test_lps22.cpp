#include "Adafruit_LPS22.h"
#include <cassert>
#include <cstdio>
#include <cmath>

static bool near(float a, float b, float eps = 0.01f) {
    return std::fabs(a - b) < eps;
}

int main() {
    // Test 1: Init I2C
    {
        Adafruit_LPS22 lps;
        assert(lps.begin_I2C(0x5C));
    }

    // Test 2: Default readings
    {
        Adafruit_LPS22 lps;
        lps.begin_I2C();
        assert(near(lps.readPressure(), 1013.25f));
        assert(near(lps.readTemperature(), 25.0f));
    }

    // Test 3: Test helpers
    {
        Adafruit_LPS22 lps;
        lps.begin_I2C();
        lps._test_set_pressure(980.5f);
        lps._test_set_temperature(18.3f);
        assert(near(lps.readPressure(), 980.5f));
        assert(near(lps.readTemperature(), 18.3f));
    }

    // Test 4: Data rate
    {
        Adafruit_LPS22 lps;
        lps.begin_I2C();
        lps.setDataRate(LPS22_RATE_25_HZ);
        assert(lps.getDataRate() == LPS22_RATE_25_HZ);
    }

    // Test 5: Data ready
    {
        Adafruit_LPS22 lps;
        lps.begin_I2C();
        assert(lps.pressureAvailable());
        lps._test_set_data_ready(false);
        assert(!lps.pressureAvailable());
        assert(!lps.temperatureAvailable());
    }

    // Test 6: Reset
    {
        Adafruit_LPS22 lps;
        lps.begin_I2C();
        lps._test_set_pressure(500.0f);
        lps._test_set_temperature(50.0f);
        lps.reset();
        assert(near(lps.readPressure(), 1013.25f));
        assert(near(lps.readTemperature(), 25.0f));
    }

    printf("test_lps22: all assertions passed\n");
    return 0;
}
