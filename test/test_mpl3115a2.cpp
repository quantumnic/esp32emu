#include "Adafruit_MPL3115A2.h"
#include <cassert>
#include <cstdio>
#include <cmath>

static bool near(float a, float b, float eps = 0.01f) {
    return std::fabs(a - b) < eps;
}

int main() {
    // Test 1: Init
    {
        Adafruit_MPL3115A2 sensor;
        assert(!sensor.isInitialized());
        assert(sensor.begin());
        assert(sensor.isInitialized());
        assert(sensor.getAddress() == 0x60);
    }

    // Test 2: Custom address
    {
        Adafruit_MPL3115A2 sensor;
        sensor.begin(0x61);
        assert(sensor.getAddress() == 0x61);
    }

    // Test 3: Pressure reading
    {
        Adafruit_MPL3115A2 sensor;
        sensor.begin();
        sensor.setPressure(98500.0f);
        assert(near(sensor.getPressure(), 98500.0f));
    }

    // Test 4: Altitude reading
    {
        Adafruit_MPL3115A2 sensor;
        sensor.begin();
        sensor.setAltitude(450.0f);
        assert(near(sensor.getAltitude(), 450.0f));
    }

    // Test 5: Temperature
    {
        Adafruit_MPL3115A2 sensor;
        sensor.begin();
        sensor.setTemperatureC(18.5f);
        assert(near(sensor.getTemperature(), 18.5f));
    }

    // Test 6: Mode switching
    {
        Adafruit_MPL3115A2 sensor;
        sensor.begin();
        sensor.setMode(Adafruit_MPL3115A2::MPL3115A2_BAROMETER);
        assert(sensor.getMode() == Adafruit_MPL3115A2::MPL3115A2_BAROMETER);
        sensor.setMode(Adafruit_MPL3115A2::MPL3115A2_ALTIMETER);
        assert(sensor.getMode() == Adafruit_MPL3115A2::MPL3115A2_ALTIMETER);
    }

    // Test 7: Oversample rate
    {
        Adafruit_MPL3115A2 sensor;
        sensor.begin();
        sensor.setOversampleRate(128);
        assert(sensor.getOversampleRate() == 128);
    }

    // Test 8: Sea-level pressure
    {
        Adafruit_MPL3115A2 sensor;
        sensor.begin();
        assert(near(sensor.getSeaPressure(), 101325.0f));
        sensor.setSeaPressure(101500.0f);
        assert(near(sensor.getSeaPressure(), 101500.0f));
    }

    printf("test_mpl3115a2: all assertions passed\n");
    return 0;
}
