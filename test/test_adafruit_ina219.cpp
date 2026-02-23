#include "Adafruit_INA219.h"
#include <cassert>
#include <cstdio>
#include <cmath>

static bool near(float a, float b, float eps = 0.1f) { return std::fabs(a - b) < eps; }

int main() {
    // Init
    {
        Adafruit_INA219 ina(0x40);
        assert(ina.begin());
        assert(ina.success());
    }

    // Default readings
    {
        Adafruit_INA219 ina;
        ina.begin();
        assert(near(ina.getBusVoltage_V(), 5.0f));
        assert(near(ina.getCurrent_mA(), 100.0f));
        assert(near(ina.getPower_mW(), 500.0f));
    }

    // Custom readings
    {
        Adafruit_INA219 ina;
        ina.begin();
        ina.setReadings(12.0f, 500.0f, 0.1f);
        assert(near(ina.getBusVoltage_V(), 12.0f));
        assert(near(ina.getCurrent_mA(), 500.0f));
        assert(near(ina.getShuntVoltage_mV(), 50.0f));
        assert(near(ina.getPower_mW(), 6000.0f));
    }

    // Calibration modes
    {
        Adafruit_INA219 ina;
        ina.begin();
        ina.setCalibration_32V_2A();
        ina.setCalibration_32V_1A();
        ina.setCalibration_16V_400mA();
    }

    // Power save
    {
        Adafruit_INA219 ina;
        ina.begin();
        assert(!ina.isPowerSave());
        ina.powerSave(true);
        assert(ina.isPowerSave());
    }

    printf("test_adafruit_ina219: all assertions passed\n");
    return 0;
}
