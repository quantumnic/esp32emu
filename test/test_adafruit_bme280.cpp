#include "Arduino.h"
#include "Adafruit_BME280.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    // Test 1: Init default
    {
        Adafruit_BME280 bme;
        assert(bme.begin());
        assert(bme.begin(0x77)); // alternate address
    }

    // Test 2: Temperature read
    {
        Adafruit_BME280 bme;
        bme.begin();
        bme.setTemperature(23.5f);
        assert(fabs(bme.readTemperature() - 23.5f) < 0.01f);
    }

    // Test 3: Humidity read
    {
        Adafruit_BME280 bme;
        bme.begin();
        bme.setHumidity(65.0f);
        assert(fabs(bme.readHumidity() - 65.0f) < 0.01f);
    }

    // Test 4: Pressure read
    {
        Adafruit_BME280 bme;
        bme.begin();
        bme.setPressure(101325.0f);
        assert(fabs(bme.readPressure() - 101325.0f) < 1.0f);
    }

    // Test 5: Altitude calculation
    {
        Adafruit_BME280 bme;
        bme.begin();
        bme.setPressure(101325.0f);
        float alt = bme.readAltitude(101325.0f);
        assert(fabs(alt) < 5.0f); // should be ~0m at sea level
    }

    // Test 6: Altitude at different pressure
    {
        Adafruit_BME280 bme;
        bme.begin();
        bme.setPressure(90000.0f); // ~1000m
        float alt = bme.readAltitude(101325.0f);
        assert(alt > 800.0f && alt < 1200.0f);
    }

    // Test 7: Sampling configuration
    {
        Adafruit_BME280 bme;
        bme.begin();
        bme.setSampling(Adafruit_BME280::MODE_FORCED,
                        Adafruit_BME280::SAMPLING_X1,
                        Adafruit_BME280::SAMPLING_X1,
                        Adafruit_BME280::SAMPLING_X1,
                        Adafruit_BME280::FILTER_OFF,
                        Adafruit_BME280::STANDBY_MS_0_5);
        // Should not crash
        assert(bme.readTemperature() >= -40.0f);
    }

    // Test 8: Sensor ID
    {
        Adafruit_BME280 bme;
        bme.begin();
        assert(bme.sensorID() == 0x60);
    }

    // Test 9: takeForcedMeasurement
    {
        Adafruit_BME280 bme;
        bme.begin();
        assert(bme.takeForcedMeasurement());
    }

    // Test 10: Multiple temperature changes
    {
        Adafruit_BME280 bme;
        bme.begin();
        for (float t = -20.0f; t <= 50.0f; t += 10.0f) {
            bme.setTemperature(t);
            assert(fabs(bme.readTemperature() - t) < 0.01f);
        }
    }

    printf("test_adafruit_bme280: all assertions passed\n");
    return 0;
}
