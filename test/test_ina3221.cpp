#include "INA3221.h"
#include <cassert>
#include <cstdio>
#include <cmath>

static bool near(float a, float b, float eps = 0.01f) {
    return std::fabs(a - b) < eps;
}

int main() {
    // Test 1: Initialization
    {
        INA3221 sensor(0x40);
        assert(!sensor.isInitialized());
        assert(sensor.begin());
        assert(sensor.isInitialized());
        assert(sensor.getAddress() == 0x40);
    }

    // Test 2: Default readings per channel
    {
        INA3221 sensor;
        sensor.begin();
        assert(near(sensor.getBusVoltage_V(0), 5.0f));
        assert(near(sensor.getBusVoltage_V(1), 3.3f));
        assert(near(sensor.getBusVoltage_V(2), 12.0f));
        assert(near(sensor.getCurrent_mA(0), 100.0f));
        assert(near(sensor.getCurrent_mA(1), 50.0f));
        assert(near(sensor.getCurrent_mA(2), 200.0f));
    }

    // Test 3: Power calculation
    {
        INA3221 sensor;
        sensor.begin();
        assert(near(sensor.getPower_mW(0), 500.0f));   // 5V * 100mA
        assert(near(sensor.getPower_mW(1), 165.0f));   // 3.3V * 50mA
        assert(near(sensor.getPower_mW(2), 2400.0f));  // 12V * 200mA
    }

    // Test 4: Set custom readings
    {
        INA3221 sensor;
        sensor.begin();
        sensor.setReadings(0, 24.0f, 1000.0f, 0.05f);
        assert(near(sensor.getBusVoltage_V(0), 24.0f));
        assert(near(sensor.getCurrent_mA(0), 1000.0f));
        assert(near(sensor.getShuntVoltage_mV(0), 50.0f));
        assert(near(sensor.getPower_mW(0), 24000.0f));
    }

    // Test 5: Channel enable/disable
    {
        INA3221 sensor;
        sensor.begin();
        assert(sensor.isChannelEnabled(0));
        sensor.disableChannel(0);
        assert(!sensor.isChannelEnabled(0));
        sensor.enableChannel(0);
        assert(sensor.isChannelEnabled(0));
    }

    // Test 6: Shunt resistance
    {
        INA3221 sensor;
        sensor.begin();
        sensor.setShuntResistance(1, 0.05f);
        assert(near(sensor.getShuntResistance(1), 0.05f));
    }

    // Test 7: Averaging mode
    {
        INA3221 sensor;
        sensor.begin();
        sensor.setAveragingMode(4);
        assert(sensor.getAveragingMode() == 4);
    }

    // Test 8: Power mode
    {
        INA3221 sensor;
        sensor.begin();
        sensor.setPowerMode(0);
        assert(sensor.getPowerMode() == 0);
    }

    // Test 9: Alert flags
    {
        INA3221 sensor;
        sensor.begin();
        assert(!sensor.isCriticalAlert(0));
        assert(!sensor.isWarningAlert(1));
        sensor.setCriticalAlert(0, true);
        sensor.setWarningAlert(1, true);
        assert(sensor.isCriticalAlert(0));
        assert(sensor.isWarningAlert(1));
    }

    // Test 10: Alert limits
    {
        INA3221 sensor;
        sensor.begin();
        sensor.setCriticalAlertLimit(0, 100.0f);
        sensor.setWarningAlertLimit(2, 50.0f);
        // No crash — limits are stored
    }

    // Test 11: Device IDs
    {
        INA3221 sensor;
        sensor.begin();
        assert(sensor.getManufacturerID() == 0x5449);
        assert(sensor.getDieID() == 0x3220);
    }

    // Test 12: Out-of-range channel safety
    {
        INA3221 sensor;
        sensor.begin();
        assert(near(sensor.getBusVoltage_V(5), 0.0f));
        assert(near(sensor.getCurrent_mA(10), 0.0f));
        assert(near(sensor.getPower_mW(99), 0.0f));
    }

    printf("test_ina3221: all assertions passed\n");
    return 0;
}
