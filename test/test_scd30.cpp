#include "SCD30.h"
#include <cassert>
#include <cstdio>
#include <cmath>

static bool near(float a, float b, float eps = 0.1f) {
    return std::fabs(a - b) < eps;
}

int main() {
    // Test 1: Init
    {
        SCD30 sensor;
        assert(!sensor.isInitialized());
        assert(sensor.begin());
        assert(sensor.isInitialized());
    }

    // Test 2: Default readings
    {
        SCD30 sensor;
        sensor.begin();
        assert(sensor.dataAvailable());
        assert(near(sensor.getCO2(), 415.0f));
        assert(near(sensor.getTemperature(), 22.0f));
        assert(near(sensor.getHumidity(), 45.0f));
    }

    // Test 3: Set readings
    {
        SCD30 sensor;
        sensor.begin();
        sensor.setReadings(800.0f, 25.5f, 60.0f);
        assert(near(sensor.getCO2(), 800.0f));
        assert(near(sensor.getTemperature(), 25.5f));
        assert(near(sensor.getHumidity(), 60.0f));
    }

    // Test 4: Measurement interval
    {
        SCD30 sensor;
        sensor.begin();
        assert(sensor.setMeasurementInterval(5));
        assert(sensor.getMeasurementInterval() == 5);
    }

    // Test 5: Altitude compensation
    {
        SCD30 sensor;
        sensor.begin();
        sensor.setAltitudeCompensation(500);
        assert(sensor.getAltitudeCompensation() == 500);
    }

    // Test 6: Temperature offset
    {
        SCD30 sensor;
        sensor.begin();
        sensor.setTemperatureOffset(2.5f);
        assert(near(sensor.getTemperatureOffset(), 2.5f));
    }

    // Test 7: Auto self-calibration
    {
        SCD30 sensor;
        sensor.begin();
        assert(sensor.getAutoSelfCalibration());
        sensor.setAutoSelfCalibration(false);
        assert(!sensor.getAutoSelfCalibration());
    }

    // Test 8: Measuring state
    {
        SCD30 sensor;
        sensor.begin();
        assert(!sensor.isMeasuring());
        sensor.beginMeasuring();
        assert(sensor.isMeasuring());
        sensor.stopMeasuring();
        assert(!sensor.isMeasuring());
    }

    // Test 9: Forced recalibration
    {
        SCD30 sensor;
        sensor.begin();
        sensor.setForcedRecalibrationFactor(450);
        assert(sensor.getForcedRecalibrationFactor() == 450);
    }

    // Test 10: Data availability
    {
        SCD30 sensor;
        sensor.begin();
        sensor.setDataReady(false);
        assert(!sensor.dataAvailable());
        sensor.setDataReady(true);
        assert(sensor.dataAvailable());
    }

    printf("test_scd30: all assertions passed\n");
    return 0;
}
