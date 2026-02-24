#include "HDC1080.h"
#include <cassert>
#include <cstdio>
#include <cmath>

static bool near(float a, float b, float eps = 0.01f) {
    return std::fabs(a - b) < eps;
}

int main() {
    // Test 1: Initialization
    {
        HDC1080 sensor;
        assert(!sensor.isInitialized());
        assert(sensor.begin());
        assert(sensor.isInitialized());
        assert(sensor.getAddress() == 0x40);
    }

    // Test 2: Custom address
    {
        HDC1080 sensor;
        sensor.begin(0x41);
        assert(sensor.getAddress() == 0x41);
    }

    // Test 3: Default readings
    {
        HDC1080 sensor;
        sensor.begin();
        assert(near(sensor.readTemperature(), 22.5f));
        assert(near(sensor.readHumidity(), 45.0f));
    }

    // Test 4: Custom readings
    {
        HDC1080 sensor;
        sensor.begin();
        sensor.setTemperature(25.3f);
        sensor.setHumidity(60.0f);
        assert(near(sensor.readTemperature(), 25.3f));
        assert(near(sensor.readHumidity(), 60.0f));
    }

    // Test 5: setReadings helper
    {
        HDC1080 sensor;
        sensor.begin();
        sensor.setReadings(-10.0f, 95.0f);
        assert(near(sensor.readTemperature(), -10.0f));
        assert(near(sensor.readHumidity(), 95.0f));
    }

    // Test 6: Heater control
    {
        HDC1080 sensor;
        sensor.begin();
        assert(!sensor.isHeaterOn());
        sensor.turnHeaterOn();
        assert(sensor.isHeaterOn());
        sensor.turnHeaterOff();
        assert(!sensor.isHeaterOn());
    }

    // Test 7: HeatUp cycle
    {
        HDC1080 sensor;
        sensor.begin();
        sensor.heatUp(10);
        assert(!sensor.isHeaterOn());  // heater turns off after heatUp
    }

    // Test 8: Device IDs
    {
        HDC1080 sensor;
        sensor.begin();
        assert(sensor.readManufacturerId() == 0x5449);
        assert(sensor.readDeviceId() == 0x1050);
    }

    // Test 9: Resolution setting
    {
        HDC1080 sensor;
        sensor.begin();
        sensor.setResolution(HDC1080::RES_11BIT, HDC1080::RES_8BIT);
        // No crash — resolution is stored
    }

    // Test 10: Battery status
    {
        HDC1080 sensor;
        sensor.begin();
        assert(near(sensor.batteryStatus(), 1.0f));
        sensor.setBatteryLow(true);
        assert(near(sensor.batteryStatus(), 0.0f));
    }

    printf("test_hdc1080: all assertions passed\n");
    return 0;
}
