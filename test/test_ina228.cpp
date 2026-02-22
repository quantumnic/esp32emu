#include "Adafruit_INA228.h"
#include <cassert>
#include <cstdio>
#include <cmath>

static bool near(float a, float b, float eps = 0.01f) {
    return std::fabs(a - b) < eps;
}

int main() {
    // Test 1: Initialization
    {
        Adafruit_INA228 ina(0x40);
        assert(ina.begin());
    }

    // Test 2: Default readings
    {
        Adafruit_INA228 ina;
        ina.begin();
        assert(near(ina.readBusVoltage(), 3.3f));
        assert(near(ina.readCurrent(), 0.1f));
        assert(near(ina.readPower(), 0.33f));
    }

    // Test 3: Test helpers
    {
        Adafruit_INA228 ina;
        ina.begin();
        ina._test_set_bus_voltage(12.0f);
        ina._test_set_current(2.5f);
        ina._test_set_power(30.0f);
        assert(near(ina.readBusVoltage(), 12.0f));
        assert(near(ina.readCurrent(), 2.5f));
        assert(near(ina.readPower(), 30.0f));
    }

    // Test 4: Energy and charge
    {
        Adafruit_INA228 ina;
        ina.begin();
        ina._test_set_energy(100.0f);
        ina._test_set_charge(50.0f);
        assert(near(ina.readEnergy(), 100.0f));
        assert(near(ina.readCharge(), 50.0f));
        ina.resetEnergy();
        ina.resetCharge();
        assert(near(ina.readEnergy(), 0.0f));
        assert(near(ina.readCharge(), 0.0f));
    }

    // Test 5: Configuration
    {
        Adafruit_INA228 ina;
        ina.begin();
        ina.setShunt(0.015f, 10.0f);
        ina.setAveragingCount(128);
        ina.setConversionTime(2048, 2048);
    }

    // Test 6: Status flags
    {
        Adafruit_INA228 ina;
        ina.begin();
        assert(!ina.alertFunctionFlag());
        assert(ina.conversionReady());
        assert(!ina.mathOverflow());
        ina._test_set_alert(true);
        ina._test_set_overflow(true);
        assert(ina.alertFunctionFlag());
        assert(ina.mathOverflow());
    }

    printf("test_ina228: all assertions passed\n");
    return 0;
}
