#include "Adafruit_HTU31D.h"
#include <cassert>
#include <cstdio>
#include <cmath>

static bool near(float a, float b, float eps = 0.01f) {
    return std::fabs(a - b) < eps;
}

int main() {
    // Test 1: Init
    {
        Adafruit_HTU31D htu;
        assert(htu.begin(0x40));
    }

    // Test 2: Default readings
    {
        Adafruit_HTU31D htu;
        htu.begin();
        assert(near(htu.readTemperature(), 25.0f));
        assert(near(htu.readHumidity(), 50.0f));
    }

    // Test 3: Test helpers
    {
        Adafruit_HTU31D htu;
        htu.begin();
        htu._test_set_temperature(30.5f);
        htu._test_set_humidity(65.2f);
        assert(near(htu.readTemperature(), 30.5f));
        assert(near(htu.readHumidity(), 65.2f));
    }

    // Test 4: Heater
    {
        Adafruit_HTU31D htu;
        htu.begin();
        assert(!htu.isHeaterEnabled());
        htu.enableHeater(true);
        assert(htu.isHeaterEnabled());
        htu.enableHeater(false);
        assert(!htu.isHeaterEnabled());
    }

    // Test 5: Reset
    {
        Adafruit_HTU31D htu;
        htu.begin();
        htu._test_set_temperature(99.0f);
        htu.enableHeater(true);
        htu.reset();
        assert(near(htu.readTemperature(), 25.0f));
        assert(!htu.isHeaterEnabled());
    }

    // Test 6: Serial & diagnostic
    {
        Adafruit_HTU31D htu;
        htu.begin();
        assert(htu.readSerial() == 0x12345678);
        htu._test_set_serial(0xDEADBEEF);
        assert(htu.readSerial() == 0xDEADBEEF);
        assert(htu.diagnosticStatus());
        htu._test_set_diagnostic(false);
        assert(!htu.diagnosticStatus());
    }

    printf("test_htu31d: all assertions passed\n");
    return 0;
}
