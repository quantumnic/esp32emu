#include "INA260.h"
#include <cassert>
#include <cstdio>
#include <cmath>

static bool near(float a, float b, float eps = 0.01f) {
    return std::fabs(a - b) < eps;
}

int main() {
    // Test 1: Init
    {
        INA260 sensor;
        assert(!sensor.isInitialized());
        assert(sensor.begin(0x40));
        assert(sensor.isInitialized());
    }

    // Test 2: Default readings
    {
        INA260 sensor;
        sensor.begin();
        assert(near(sensor.readBusVoltage(), 5.0f));
        assert(near(sensor.readCurrent(), 100.0f));
        assert(near(sensor.readPower(), 500.0f));
    }

    // Test 3: Set readings
    {
        INA260 sensor;
        sensor.begin();
        sensor.setReadings(3.3f, 250.0f);
        assert(near(sensor.readBusVoltage(), 3.3f));
        assert(near(sensor.readCurrent(), 250.0f));
        assert(near(sensor.readPower(), 825.0f));
    }

    // Test 4: Averaging
    {
        INA260 sensor;
        sensor.begin();
        sensor.setAveragingCount(INA260_AveragingCount::AVG_64);
        assert(sensor.getAveragingCount() == INA260_AveragingCount::AVG_64);
    }

    // Test 5: Conversion time
    {
        INA260 sensor;
        sensor.begin();
        sensor.setVoltageConversionTime(INA260_ConversionTime::TIME_8_244_ms);
        assert(sensor.getVoltageConversionTime() == INA260_ConversionTime::TIME_8_244_ms);
        sensor.setCurrentConversionTime(INA260_ConversionTime::TIME_204_us);
        assert(sensor.getCurrentConversionTime() == INA260_ConversionTime::TIME_204_us);
    }

    // Test 6: Mode
    {
        INA260 sensor;
        sensor.begin();
        sensor.setMode(INA260_MeasurementMode::TRIGGERED);
        assert(sensor.getMode() == INA260_MeasurementMode::TRIGGERED);
    }

    // Test 7: Conversion ready
    {
        INA260 sensor;
        sensor.begin();
        assert(sensor.conversionReady());
        sensor.setConversionReady(false);
        assert(!sensor.conversionReady());
    }

    // Test 8: Alert
    {
        INA260 sensor;
        sensor.begin();
        sensor.setAlertType(1);
        sensor.setAlertLimit(15.0f);
        assert(!sensor.alertFunctionFlag());
        sensor.setAlertFlag(true);
        assert(sensor.alertFunctionFlag());
    }

    printf("test_ina260: all assertions passed\n");
    return 0;
}
