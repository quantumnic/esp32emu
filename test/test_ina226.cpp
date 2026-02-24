#include "INA226.h"
#include <cassert>
#include <cstdio>
#include <cmath>

static bool near(float a, float b, float eps = 0.01f) {
    return std::fabs(a - b) < eps;
}

int main() {
    // Test 1: Init
    {
        INA226 sensor;
        assert(!sensor.isInitialized());
        assert(sensor.begin());
        assert(sensor.isInitialized());
        assert(sensor.getAddress() == 0x40);
    }

    // Test 2: Custom address
    {
        INA226 sensor(0x44);
        sensor.begin();
        assert(sensor.getAddress() == 0x44);
    }

    // Test 3: Calibration and current
    {
        INA226 sensor;
        sensor.begin();
        sensor.calibrate(0.1f, 3.2f); // 100mOhm shunt, 3.2A max
        assert(sensor.isCalibrated());

        sensor.setBusVoltage(12.0f);
        sensor.setShuntVoltage(50.0f); // 50mV -> 0.5A
        assert(near(sensor.getBusVoltage(), 12.0f));
        assert(near(sensor.getShuntVoltage(), 50.0f));
        assert(near(sensor.getCurrent(), 0.5f));
        assert(near(sensor.getPower(), 6.0f));
    }

    // Test 4: Uncalibrated returns zero current
    {
        INA226 sensor;
        sensor.begin();
        sensor.setBusVoltage(5.0f);
        sensor.setShuntVoltage(10.0f);
        assert(near(sensor.getCurrent(), 0.0f));
    }

    // Test 5: Average config
    {
        INA226 sensor;
        sensor.begin();
        assert(sensor.getAverage() == INA226::AVG_1);
        sensor.setAverage(INA226::AVG_256);
        assert(sensor.getAverage() == INA226::AVG_256);
    }

    // Test 6: Conversion times
    {
        INA226 sensor;
        sensor.begin();
        sensor.setBusConversionTime(6);
        sensor.setShuntConversionTime(5);
        assert(sensor.getBusConversionTime() == 6);
        assert(sensor.getShuntConversionTime() == 5);
    }

    // Test 7: Mode
    {
        INA226 sensor;
        sensor.begin();
        assert(sensor.getMode() == 7); // continuous
        sensor.setMode(0); // power-down
        assert(sensor.getMode() == 0);
    }

    // Test 8: Over-current alert
    {
        INA226 sensor;
        sensor.begin();
        sensor.calibrate(0.1f, 3.2f);
        sensor.setAlertType(INA226::ALERT_OVER_CURRENT);
        sensor.setAlertLimit(1.0f);

        sensor.setShuntVoltage(50.0f); // 0.5A
        assert(!sensor.alertTriggered());

        sensor.setShuntVoltage(150.0f); // 1.5A
        assert(sensor.alertTriggered());
    }

    // Test 9: Over-voltage alert
    {
        INA226 sensor;
        sensor.begin();
        sensor.calibrate(0.1f, 3.2f);
        sensor.setAlertType(INA226::ALERT_OVER_VOLTAGE);
        sensor.setAlertLimit(15.0f);

        sensor.setBusVoltage(12.0f);
        assert(!sensor.alertTriggered());

        sensor.setBusVoltage(16.0f);
        assert(sensor.alertTriggered());
    }

    // Test 10: Reset
    {
        INA226 sensor;
        sensor.begin();
        sensor.calibrate(0.1f, 3.2f);
        sensor.setBusVoltage(5.0f);
        sensor.setAverage(INA226::AVG_64);
        sensor.reset();
        assert(!sensor.isCalibrated());
        assert(near(sensor.getBusVoltage(), 0.0f));
        assert(sensor.getAverage() == INA226::AVG_1);
    }

    printf("test_ina226: all assertions passed\n");
    return 0;
}
