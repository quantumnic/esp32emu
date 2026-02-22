#include "Adafruit_PCT2075.h"
#include <cassert>
#include <cstdio>
#include <cmath>

static bool near(float a, float b, float eps = 0.01f) {
    return std::fabs(a - b) < eps;
}

int main() {
    // Test 1: Initialization
    {
        Adafruit_PCT2075 sensor(0x37);
        assert(sensor.begin());
    }

    // Test 2: Default temperature
    {
        Adafruit_PCT2075 sensor;
        sensor.begin();
        assert(near(sensor.getTemperature(), 25.0f));
    }

    // Test 3: Set temperature
    {
        Adafruit_PCT2075 sensor;
        sensor.begin();
        sensor._test_set_temperature(42.5f);
        assert(near(sensor.getTemperature(), 42.5f));
    }

    // Test 4: Thresholds
    {
        Adafruit_PCT2075 sensor;
        sensor.begin();
        sensor.setHighTemperatureThreshold(85.0f);
        sensor.setTemperatureHysteresis(80.0f);
        assert(near(sensor.getHighTemperatureThreshold(), 85.0f));
        assert(near(sensor.getTemperatureHysteresis(), 80.0f));
    }

    // Test 5: Configuration
    {
        Adafruit_PCT2075 sensor;
        sensor.begin();
        sensor.setIdleTime(3);
        sensor.setFaultCount(4);
        sensor.setAlertMode(1);
        sensor.setAlertPolarity(1);
        assert(sensor.getIdleTime() == 3);
        assert(sensor.getFaultCount() == 4);
        assert(sensor.getAlertMode() == 1);
        assert(sensor.getAlertPolarity() == 1);
    }

    printf("test_pct2075: all assertions passed\n");
    return 0;
}
